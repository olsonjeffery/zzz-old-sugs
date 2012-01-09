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
  JSRuntime* rt = sugs::core::js::initRuntime(vmMemSize);
  this->_jsEnv = sugs::core::js::initContext(rt);

  // load core libs
  this->loadConfig(this->_config);
  this->loadSugsLibraries(this->_config.paths);
  this->loadComponents(this->_config);
}


bool doComponentWork(jsEnv jsEnv, sugsConfig config, std::list<sugs::core::ext::Component*> comps)
{
  std::list<sugs::core::ext::Component*>::iterator it;
  for(it = comps.begin(); it != comps.end(); it++)
  {
    sugs::core::ext::Component* c = *it;
    bool result = c->doWork(jsEnv, config);
    if (result == false) {
      return false;
    }
  }
  return true;
}

void Worker::kill() {
  this->_receivedKillSignal = true;
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
      continueIterating = doComponentWork(this->_jsEnv, this->_config, this->_components);
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

  JSObject* workerWrapper = JSVAL_TO_OBJECT(sugs::common::jsutil::pullPropertyFromSugsConfigInGlobal(cx, global, "workerRef"));
  Worker* worker = (Worker*)JS_GetPrivate(cx, workerWrapper);
  MessageExchange* msgEx = worker->getMessageExchange();

  JSString* workerIdStr = JSVAL_TO_STRING(sugs::common::jsutil::pullPropertyFromSugsConfigInGlobal(cx, global, "myWorkerId"));
  std::string myWorkerId(JS_EncodeString(cx, workerIdStr));

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

  JSObject* workerWrapper = JSVAL_TO_OBJECT(sugs::common::jsutil::pullPropertyFromSugsConfigInGlobal(cx, global, "workerRef"));
  Worker* worker = (Worker*)JS_GetPrivate(cx, workerWrapper);
  JSString* workerIdStr = JSVAL_TO_STRING(sugs::common::jsutil::pullPropertyFromSugsConfigInGlobal(cx, global, "myWorkerId"));
  std::string myWorkerId(JS_EncodeString(cx, workerIdStr));

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

  JSObject* workerWrapper = JSVAL_TO_OBJECT(sugs::common::jsutil::pullPropertyFromSugsConfigInGlobal(cx, global, "workerRef"));
  Worker* worker = (Worker*)JS_GetPrivate(cx, workerWrapper);
  JSString* workerIdStr = JSVAL_TO_STRING(sugs::common::jsutil::pullPropertyFromSugsConfigInGlobal(cx, global, "myWorkerId"));
  std::string myWorkerId(JS_EncodeString(cx, workerIdStr));

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

void bindMessageExchangeFunctions(JSContext* cx, JSObject* global) {
  printf("binding message exchange funcions..\n");
  JS_DefineFunctions(cx, global, messagingFunctionSpec);
}

MessageExchange* Worker::getMessageExchange() {
  return this->_msgEx;
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
  result = sugs::core::js::findAndExecuteScript("sugs.coffee", paths, this->_jsEnv.cx, this->_jsEnv.global);
  if(result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }

  bindMessageExchangeFunctions(this->_jsEnv.cx, this->_jsEnv.global);

  printf(">>> finishing Worker::loadSugsLibraries()!\n");
}

void bindWorkerToConfig(JSContext* cx, JSObject* sugsConfig, Worker* workerP) {
  JSObject* workerWrapperObj = JS_NewObject(cx, sugs::common::jsutil::getDefaultClassDef(), NULL, NULL);
  if(!JS_SetPrivate(cx, workerWrapperObj,workerP)) {
    printf("bindWorkerToConfig: unable to bind Worker pointer to wrapper obj\n");
    exit(EXIT_FAILURE);
  }
  jsval wrapperVal = OBJECT_TO_JSVAL(workerWrapperObj);
  if(!JS_SetProperty(cx, sugsConfig, "workerRef", &wrapperVal)) {
    printf("bindWorkerToConfig: unable to add Worker wrapper to sugsConfig\n");
    exit(EXIT_FAILURE);
  }
}

void Worker::loadConfig(sugsConfig config) {
  JSObject* sugsConfigObj = JS_NewObject(this->_jsEnv.cx, NULL, NULL, NULL);

  // the members of config that we're converting...
  jsval screenWidthVal = INT_TO_JSVAL(config.screenWidth);
  if(!JS_SetProperty(this->_jsEnv.cx, sugsConfigObj, "screenWidth", &screenWidthVal)) {
    printf("Failed to convert native sugsConfig.screenWidth and store in sugsConfig JSObject\n");
    exit(EXIT_FAILURE);
  }
  jsval screenHeightVal = INT_TO_JSVAL(config.screenHeight);
  if(!JS_SetProperty(this->_jsEnv.cx, sugsConfigObj, "screenHeight", &screenHeightVal)) {
    printf("Failed to convert native sugsConfig.screenHeight and store in sugsConfig JSObject\n");
    exit(EXIT_FAILURE);
  }
  jsval colorDepthVal = INT_TO_JSVAL(config.colorDepth);
  if(!JS_SetProperty(this->_jsEnv.cx, sugsConfigObj, "colorDepth", &colorDepthVal)) {
    printf("Failed to convert native sugsConfig.colorDepth and store in sugsConfig JSObject\n");
    exit(EXIT_FAILURE);
  }

  jsval pathVals[config.paths.length];
  for(int ctr = 0; ctr < config.paths.length;ctr++) {
    const char* cStr = config.paths.paths[ctr].c_str();
    JSString* pathStr = JS_NewStringCopyN(this->_jsEnv.cx, cStr, strlen(cStr));
    pathVals[ctr] =STRING_TO_JSVAL(pathStr);
  }
  JSObject* pathsArrObj = JS_NewArrayObject(this->_jsEnv.cx, config.paths.length, pathVals);
  jsval pathsArrVal = OBJECT_TO_JSVAL(pathsArrObj);
  if(!JS_SetProperty(this->_jsEnv.cx, sugsConfigObj, "paths", &pathsArrVal)) {
    printf("Failed to convert native sugsConfig.paths and store in sugsConfig JSObject\n");
    exit(EXIT_FAILURE);
  }

  const char* workerIdCStr = this->_workerId.c_str();
  JSString* myWorkerIdStr = JS_NewStringCopyN(this->_jsEnv.cx, workerIdCStr, strlen(workerIdCStr));
  jsval myWorkerIdVal = STRING_TO_JSVAL(myWorkerIdStr);
  if(!JS_SetProperty(this->_jsEnv.cx, sugsConfigObj, "myWorkerId", &myWorkerIdVal)) {
    printf("Failed to convert worker's _workerId and store in sugsConfig JSObject\n");
    exit(EXIT_FAILURE);
  }

  bindWorkerToConfig(this->_jsEnv.cx, sugsConfigObj, this);

  JSString* customJsonStr = JS_NewStringCopyN(this->_jsEnv.cx, config.customJson, strlen(config.customJson));
  jsval customJsonVal = STRING_TO_JSVAL(customJsonStr);
  if(!JS_SetProperty(this->_jsEnv.cx, sugsConfigObj, "customJson", &customJsonVal)) {
    printf("Failed to convert native sugsConfig.customJson and store in sugsConfig JSObject\n");
    exit(EXIT_FAILURE);
  }

  jsval sugsConfigVal = OBJECT_TO_JSVAL(sugsConfigObj);
  if(!JS_SetProperty(this->_jsEnv.cx, this->_jsEnv.global, "sugsConfig", &sugsConfigVal)) {
    printf("Failed to convert native sugsConfig and load into global object\n");
    exit(EXIT_FAILURE);
  }

  // can't do this until after the config is bound to the global...
  sugs::core::js::executeJavascriptSnippet("(function() {sugsConfig.custom = JSON.parse(sugsConfig.customJson);})();", this->_jsEnv.cx, this->_jsEnv.global);

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

void Worker::loadComponents(sugsConfig config)
{
  printf("LOADING COMPONENTS...\n");
  std::list<sugs::core::ext::Component*>::iterator it;
  for(it = this->_components.begin(); it != this->_components.end(); it++)
  {
    printf("FOUND COMPONENT TO LOAD..\n");
    sugs::core::ext::Component* c = *it;
    c->registerNativeFunctions(this->_jsEnv, config);
  }
}

}}} // namespace sugs::core::worker
