/* Copyright 2011 Jeffery Olson <olson.jeffery@gmail.com>. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this list of
 *     conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice, this list
 *     of conditions and the following disclaimer in the documentation and/or other materials
 *     provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY JEFFERY OLSON <OLSON.JEFFERY@GMAIL.COM> ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * JEFFERY OLSON <OLSON.JEFFERY@GMAIL.COM> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of Jeffery Olson <olson.jeffery@gmail.com>.
 *
 */

#include "worker.hpp"

namespace sugs {
namespace core {
namespace worker {

const int vmMemSize = (((1024L) * 1024L) * 1024L);
void Worker::init() {
  // load core libs
  this->setupEnvironment();
  this->loadConfig(this->_paths, this->_dataJson);
  this->loadSugsLibraries(this->_paths);

  // so we should have bound all of the core native stuff
  // we need to by now, and components are about to
  // be loaded. most likely, the last component is
  // our ScriptRunner.. so everything needs to loaded
  // by then, since its sole jerb is to run the entry
  // point script.
  this->loadComponents(this->_paths);
}

void Worker::setupEnvironment()
{
  JSRuntime* rt = sugs::core::js::initRuntime(vmMemSize);
  this->_jsEnv = sugs::core::js::initContext(rt);

  contextPrivateData* cxData = new contextPrivateData();
  cxData->worker = this;
  JS_SetContextPrivate(this->_jsEnv.cx, cxData);
}

bool doComponentWork(jsEnv jsEnv, pathStrings paths, std::list<sugs::core::ext::Component*> comps)
{
  std::list<sugs::core::ext::Component*>::iterator it;
  for(it = comps.begin(); it != comps.end(); it++)
  {
    sugs::core::ext::Component* c = *it;
    bool result = c->doWork(jsEnv, paths);
    if (result == false) {
      return false;
    }
  }
  return true;
}

void workerCallback(void* wpP) {
  workerPayload* payload = (workerPayload*)wpP;
  sugs::core::worker::Worker* worker = (sugs::core::worker::Worker*)(payload->worker);

  worker->init();
  worker->begin();
  std::cout << "NOW EXITING " << worker->getWorkerId() << std::endl;
}

void Worker::start(bool runInNewThread) {
  this->_runInNewThread = runInNewThread;
  workerPayload* wpP = new workerPayload;
  workerPayload wp = { (void*)this };
  *wpP = wp;

  if(this->_runInNewThread) {
    sf::Thread* workerThread;
    this->_workerThread = new sf::Thread(&workerCallback, wpP);
    this->_workerThread->Launch();
  }
  else {
    workerCallback((void*)wpP);
  }
}

void Worker::kill() {
  this->_receivedKillSignal = true;
  if (this->_runInNewThread) {
    this->_workerThread->Wait();
  }
}

bool Worker::receivedKillSignal() {
  return this->_receivedKillSignal;
}

// still need to encapsulate threading in here
void Worker::begin() {
  bool continueIterating = true;
  // we the loop processes incoming messages, and lets
  // components "do their work". If any of the components,
  // while being processed, return false, we short circuit
  // the doComponentWork loop and return, then exit out,
  // then this while loop will end, as well.
  if (continueIterating && !this->receivedKillSignal()) {
    while (continueIterating && !this->receivedKillSignal()) {
      this->processPendingMessages();
      continueIterating = doComponentWork(this->_jsEnv, this->_paths, this->_components);
    }
  }
}

static JSBool
native_subscribe(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* global = JS_GetGlobalObject(cx);
  JSString* msgIdStr;
  if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &msgIdStr)) {
    JS_ReportError(cx, "native_subscribe: failed to convert arguments");
    return JS_FALSE;
  }

  char* msgIdCStr = JS_EncodeString(cx, msgIdStr);
  std::string msgId(msgIdCStr);

  Worker* worker = ((contextPrivateData*)JS_GetContextPrivate(cx))->worker;
  MessageExchange* msgEx = worker->getMessageExchange();

  std::string myWorkerId = worker->getWorkerId();

  msgEx->addSubscription(myWorkerId, msgId);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool
native_publish_broadcast(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* global = JS_GetGlobalObject(cx);

  JSString* msgIdStr;
  JSString* jsonDataStr;
  if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "SS", &msgIdStr, &jsonDataStr)) {
    JS_ReportError(cx, "native_publish_broadcast: failed to convert arguments");
    return JS_FALSE;
  }
  std::string msgId(JS_EncodeString(cx, msgIdStr));
  std::string jsonData(JS_EncodeString(cx, jsonDataStr));

  Worker* worker = ((contextPrivateData*)JS_GetContextPrivate(cx))->worker;
  std::string myWorkerId = worker->getWorkerId();

  worker->getMessageExchange()->publish(myWorkerId, msgId, jsonData);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool
native_publish_single_target(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* global = JS_GetGlobalObject(cx);

  JSString* targetStr;
  JSString* msgIdStr;
  JSString* jsonDataStr;
  if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "SSS", &targetStr, &msgIdStr, &jsonDataStr)) {
    JS_ReportError(cx, "native_publish_broadcast: failed to convert arguments");
    return JS_FALSE;
  }
  std::string target(JS_EncodeString(cx, targetStr));
  std::string msgId(JS_EncodeString(cx, msgIdStr));
  std::string jsonData(JS_EncodeString(cx, jsonDataStr));

  Worker* worker = ((contextPrivateData*)JS_GetContextPrivate(cx))->worker;
  std::string myWorkerId = worker->getWorkerId();

  worker->getMessageExchange()->publish(target, myWorkerId, msgId, jsonData);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSFunctionSpec messagingFunctionSpec[] = {
  JS_FS("__native_subscribe", native_subscribe, 1, 0),
  JS_FS("__native_publish_broadcast", native_publish_broadcast, 2, 0),
  JS_FS("__native_publish_single_target", native_publish_single_target, 3, 0),
  JS_FS_END
};

static JSBool
native_global_getPaths(JSContext* cx, uintN argc, jsval* vp)
{
  printf("entering sugs.api.core.getPaths()\n");
  contextPrivateData* data = (contextPrivateData*)JS_GetContextPrivate(cx);
  printf("gonna dive into worker..");
  pathStrings paths = data->worker->getPaths();
  printf ("Number of paths from our worker? %d\n", paths.length);

  jsval elems[paths.length];
  for(int ctr = 0;ctr < paths.length;ctr++)
  {
    std::string path = paths.paths[ctr];
    elems[ctr] = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, path.c_str()));
  }

  JSObject* pathsArrObj = JS_NewArrayObject(cx, paths.length, elems);
  jsval pathsVal = OBJECT_TO_JSVAL(pathsArrObj);

  JS_SET_RVAL(cx, vp, pathsVal);
  return JS_TRUE;
}

static JSFunctionSpec coreUtilFunctionSpec[] = {
  JS_FS("getPaths", native_global_getPaths, 0, 0),
  JS_FS_END
};

void bindCoreUtilFunctions(jsEnv jsEnv)
{
  printf("binding core util funcs..\n");

  // this all needs to generalized and/or encapsulated in a pure-native
  // embedObjectsInNamespace..
  JSObject* sugsObj = JS_NewObject(jsEnv.cx, sugs::common::jsutil::getDefaultClassDef(), NULL, NULL);
  JSObject* apiObj = JS_NewObject(jsEnv.cx, sugs::common::jsutil::getDefaultClassDef(), NULL, NULL);
  JSObject* coreObj;
  sugs::common::jsutil::newJSObjectFromFunctionSpec(jsEnv.cx, coreUtilFunctionSpec, &coreObj);

  jsval coreVal = OBJECT_TO_JSVAL(coreObj);
  JS_SetProperty(jsEnv.cx, apiObj, "core", &coreVal);
  jsval apiVal = OBJECT_TO_JSVAL(apiObj);
  JS_SetProperty(jsEnv.cx, sugsObj, "api", &apiVal);
  jsval sugsVal = OBJECT_TO_JSVAL(sugsObj);
  JS_SetProperty(jsEnv.cx, jsEnv.global, "sugs", &sugsVal);
  //sugs::common::jsutil::embedObjectInNamespace(jsEnv.cx, jsEnv.global, jsEnv.global, "sugs.api.core", coreObj);
  printf("done binding core util funcs...\n");
}

void bindMessageExchangeFunctions(JSContext* cx, JSObject* global) {
  printf("binding message exchange funcions..\n");
  JS_DefineFunctions(cx, global, messagingFunctionSpec);
}

MessageExchange* Worker::getMessageExchange() {
  return this->_msgEx;
}

bool Worker::runningInNewThread()
{
  return this->_runInNewThread;
}

static JSBool
native_worker_getId(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* global = JS_GetGlobalObject(cx);
  JSObject* thisObj = JS_THIS_OBJECT(cx, vp);
  Worker* worker = (Worker*)JS_GetPrivate(cx, thisObj);
  JSString* workerIdStr = JS_NewStringCopyZ(cx, worker->getWorkerId().c_str());
  jsval workerIdVal = STRING_TO_JSVAL(workerIdStr);
  JS_SET_RVAL(cx, vp, workerIdVal);
  return JS_TRUE;
}

static JSBool
native_worker_kill(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* thisObj = JS_THIS_OBJECT(cx, vp);
  Worker* worker = (Worker*)JS_GetPrivate(cx, thisObj);
  worker->kill();
  // don't delete the Worker if it's running as blocking
  // the thread (should only be the worker running on
  // the main thread), let application code handle
  // cleanup..
  if (worker->runningInNewThread()) {
    delete worker;
  }

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool
native_worker_getPaths(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* thisObj = JS_THIS_OBJECT(cx, vp);
  Worker* worker = (Worker*)JS_GetPrivate(cx, thisObj);
  pathStrings paths = worker->getPaths();

  jsval elems[paths.length];
  for(int ctr = 0;ctr < paths.length;ctr++)
  {
    std::string path = paths.paths[ctr];
    elems[ctr] = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, path.c_str()));
  }

  JSObject* pathsArrObj = JS_NewArrayObject(cx, paths.length, elems);
  jsval pathsVal = OBJECT_TO_JSVAL(pathsArrObj);

  JS_SET_RVAL(cx, vp, pathsVal);
  return JS_TRUE;
}

static JSBool
native_worker_getData(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* thisObj = JS_THIS_OBJECT(cx, vp);
  Worker* worker = (Worker*)JS_GetPrivate(cx, thisObj);
  jsval dataVal = OBJECT_TO_JSVAL(worker->getData());
  JS_SET_RVAL(cx, vp, dataVal);
  return JS_TRUE;
}

static JSFunctionSpec workerWrapperFunctionSpec[] = {
  JS_FS("kill", native_worker_kill, 0, 0),
  JS_FS("getPaths", native_worker_getPaths, 0, 0),
  JS_FS("getId", native_worker_getId, 0, 0),
  JS_FS("getData", native_worker_getData, 0, 0),
  JS_FS_END
};

static JSObject*
createWrapperForWorker(JSContext* cx, Worker* newWorker)
{
  JSObject* workerWrapperObj;
  if (!sugs::common::jsutil::newJSObjectFromFunctionSpec(cx, workerWrapperFunctionSpec, &workerWrapperObj)) {
    JS_ReportError(cx, "native_worker_spawn: failed to create worker wrapper obj, aborting");
    return JS_FALSE;
  }
  if (!JS_SetPrivate(cx, workerWrapperObj, (void*)newWorker)) {
    printf ("native_worker_spawn: failure to set private worker pointer on wrapper obj\n");
    return JS_FALSE;
  }

  return workerWrapperObj;
}

static JSBool
native_worker_spawn(JSContext* cx, uintN argc, jsval* vp)
{
  JSString* prefixStr;
  JSObject* componentArr;
  JSString* dataJson;
  if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "SoS", &prefixStr, &componentArr, &dataJson)) {
    JS_ReportError(cx, "native_worker_spawn: failed to convert arguments");
    return JS_FALSE;
  }
  std::string prefix(JS_EncodeString(cx, prefixStr));

  JSObject* global = JS_GetGlobalObject(cx);
  Worker* worker = ((contextPrivateData*)JS_GetContextPrivate(cx))->worker;

  MessageExchange* msgEx = worker->getMessageExchange();
  pathStrings paths = worker->getPaths();

  // instantiate new worker
  Worker* newWorker = new Worker(msgEx, prefix, paths, std::string(JS_EncodeString(cx, dataJson)));

  JSObject* workerWrapperObj = createWrapperForWorker(cx, newWorker);
  jsval workerWrapperVal = OBJECT_TO_JSVAL(workerWrapperObj);

  // add components to new worker
  jsuint compsLength;
  if(!JS_GetArrayLength(cx, componentArr, &compsLength)) {
    printf("native_worker_spawn: failure to pull camps length from camps obj\n");
      exit(EXIT_FAILURE);
  }
  for(jsint ctr = 0;ctr < compsLength;ctr++) {
      jsval entryVal;
      if (!JS_GetElement(cx, componentArr, ctr, &entryVal)) {
        printf("native_worker_spawn: failure to pull entryVal from compsObj.. ctr pos: %d\n", ctr);
        exit(EXIT_FAILURE);
      }
      JSObject* componentObj = JSVAL_TO_OBJECT(entryVal);
      jsval compNameVal;
      if (!JS_GetProperty(cx, componentObj, "name", &compNameVal)) {
        printf("native_worker_spawn: failed to pull name val from component object");
        exit(EXIT_FAILURE);
      }
      JSString* compNameStr = JSVAL_TO_STRING(compNameVal);
      std::string compName(JS_EncodeString(cx, compNameStr));

      jsval compConfigJsonVal;
      if (!JS_GetProperty(cx, componentObj, "configJson", &compConfigJsonVal)) {
        printf("native_worker_spawn: failed to pull name val from component object");
        exit(EXIT_FAILURE);
      }
      JSString* compConfigJsonStr = JSVAL_TO_STRING(compConfigJsonVal);
      std::string compConfigJson(JS_EncodeString(cx, compConfigJsonStr));

      sugs::core::ext::ComponentFactory* factory = sugs::core::ext::ComponentLibrary::getComponentFactory(compName);
      sugs::core::ext::ComponentPair pair(factory, compConfigJson);

      newWorker->addComponentPair(pair);
  }

  // start new worker in a new OS thread
  newWorker->start(true);
  JS_SET_RVAL(cx, vp, workerWrapperVal);
  return JS_TRUE;
}

static JSBool
native_worker_getCurrentWorker(JSContext* cx, uintN argc, jsval* vp)
{
  contextPrivateData* cxData = (contextPrivateData*)JS_GetContextPrivate(cx);
  JSObject* wrapper = createWrapperForWorker(cx, cxData->worker);
  jsval wrapperVal = OBJECT_TO_JSVAL(wrapper);
  JS_SET_RVAL(cx, vp, wrapperVal);
  return JS_TRUE;
}

static JSFunctionSpec workerSpawnFunctionSpec[] = {
  JS_FS("spawn", native_worker_spawn, 2, 0),
  JS_FS("getCurrent", native_worker_getCurrentWorker, 2, 0),
  JS_FS_END
};

void bindWorkerSpawnFunctions(jsEnv jsEnv) {
  JSObject* workerNativeObj;
  sugs::common::jsutil::newJSObjectFromFunctionSpec(jsEnv.cx, workerSpawnFunctionSpec, &workerNativeObj);
  sugs::common::jsutil::embedObjectInNamespace(jsEnv.cx, jsEnv.global, jsEnv.global, "sugs.api.worker", workerNativeObj);
}

pathStrings Worker::getPaths() {
  return this->_paths;
}

std::string Worker::getWorkerId() {
  return this->_workerId;
}

JSObject* Worker::getData()
{
  return this->_dataObj;
}

void Worker::loadSugsLibraries(pathStrings paths) {
  predicateResult result;
  // Load up our core dependencies
  result = sugs::core::js::findAndExecuteScript("underscore.js", paths, this->_jsEnv.cx, this->_jsEnv.global);
  if(result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }
  result = sugs::core::js::findAndExecuteScript("coffee-script.js", paths, this->_jsEnv.cx, this->_jsEnv.global);
  if(result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }
  result = sugs::core::js::findAndExecuteScript("sugar.min.js", paths, this->_jsEnv.cx, this->_jsEnv.global);
  if(result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }

  bindCoreUtilFunctions(this->_jsEnv);

  result = sugs::core::js::findAndExecuteScript("sugs.coffee", paths, this->_jsEnv.cx, this->_jsEnv.global);
  if(result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }

  bindMessageExchangeFunctions(this->_jsEnv.cx, this->_jsEnv.global);
  bindWorkerSpawnFunctions(this->_jsEnv);

  printf(">>> finishing Worker::loadSugsLibraries()!\n");
}

void Worker::loadConfig(pathStrings paths, std::string dataJsonStr) {
  // just keeping this for when we make this the (data) passed
  // into a startup.. ?
  predicateResult customResult = sugs::core::js::executeJavascriptSnippet(std::string("(function() { return "+dataJsonStr+";})()").c_str(), this->_jsEnv.cx, this->_jsEnv.global);
  jsval customVal = customResult.optionalRetVal;
  JSObject* dataObj = JSVAL_TO_OBJECT(customVal);
  this->_dataObj = dataObj;

  printf("sugsConfig added to global\n");
}

void Worker::teardown() {
  sugs::core::js::teardownContext(this->_jsEnv.cx);
}

void Worker::processPendingMessages()
{
  while(this->_msgEx->messagesPendingFor(this->_workerId))
  {
    PubSubMsg msg = this->_msgEx->unshiftNextMsgFor(this->_workerId);
    const char* msgIdCStr = msg.getMsgId().c_str();
    JSString* msgIdStr = JS_NewStringCopyN(this->_jsEnv.cx, msgIdCStr, strlen(msgIdCStr));
    const char* jsonDataCStr = msg.getJsonData().c_str();
    JSString* jsonDataStr = JS_NewStringCopyN(this->_jsEnv.cx, jsonDataCStr, strlen(jsonDataCStr));
    jsval argv[2];
    argv[0] = STRING_TO_JSVAL(msgIdStr);
    argv[1] = STRING_TO_JSVAL(jsonDataStr);
    jsval rVal;
    //printf("NEW MSG: %s in %s\n", msgIdCStr, this->_workerId.c_str());
    JS_CallFunctionName(this->_jsEnv.cx, this->_jsEnv.global, "__processIncomingMessage", 2, argv, &rVal);
  }
}

void Worker::addComponent(sugs::core::ext::Component* c)
{
  this->_components.push_back(c);
}

void Worker::addComponentPair(sugs::core::ext::ComponentPair pair)
{
  this->_componentPairs.push_back(pair);
}

void Worker::loadComponents(pathStrings paths)
{
  printf("LOADING COMPONENTS...\n");
  std::list<sugs::core::ext::ComponentPair>::iterator it;
  for(it = this->_componentPairs.begin(); it != this->_componentPairs.end(); it++)
  {
    sugs::core::ext::ComponentPair pair = *it;
    sugs::core::ext::ComponentFactory* factory = pair.getComponentFactory();
    printf("LOADING %s COMPONENT\n", factory->getName().c_str());
    std::string configJson = pair.getConfigJson();


    // this is really hacky/not safe.. should probably turn the configJson into
    // a proper JSObject* as a string and pass it in to JSON.parse, but I don't
    // have anything in place to facilitate this. quite a kludge.

    predicateResult result = sugs::core::js::executeJavascriptSnippet(std::string("(function() { return "+configJson+";})()").c_str(), this->_jsEnv.cx, this->_jsEnv.global);
    JSObject* configJsonObj = JSVAL_TO_OBJECT(result.optionalRetVal);
    sugs::core::ext::Component* c = factory->create(this->_jsEnv, configJsonObj);
    c->registerNativeFunctions(this->_jsEnv, paths);
    this->addComponent(c);
  }
}

}}} // namespace sugs::core::worker
