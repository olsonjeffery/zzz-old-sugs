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

#include "jssetup.hpp"

/* NATIVE UTIL FUNCTIONS */
JSBool reformer_native_puts(JSContext* cx, uintN argc, jsval* vp)
{
  JSString* text;

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &text)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "reformer_native_puts: Error parsing arg");
      return JS_FALSE;
  }

  char* textStr = JS_EncodeString(cx, text);
  printf("%s\n", textStr);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);  /* return undefined */
  return JS_TRUE;
}
JSBool reformer_native_executeScript(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* global;
  JSString* text;

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "oS", &global, &text)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "reformer_native_executeScript -- error converting path arg");
      return JS_FALSE;
  }

  char* path = JS_EncodeString(cx, text);

  predicateResult result = executeFullPathJavaScript(path, cx, global);
  if (result.result == JS_FALSE) {
    //JS_ReportError(cx, "reformer_native_executeScript -- error running js script '%s': %s", path, result.message);
    return JS_FALSE;
  }

  JS_SET_RVAL(cx, vp, result.optionalRetVal);  /* return undefined */
  return JS_TRUE;
}
JSBool reformer_native_executeCoffeeScript(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* global;
  JSString* text;

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "oS", &global, &text)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "reformer_native_executeCoffeeScript -- error converting path arg");
      return JS_FALSE;
  }

  char* path = JS_EncodeString(cx, text);

  predicateResult result = executeFullPathCoffeeScript(path, cx, global);
  if (result.result == JS_FALSE) {
    //JS_ReportError(cx, "reformer_native_executeCoffeeScript -- error running coffee script '%s': %s", path, result.message);
    return JS_FALSE;
  }

  JS_SET_RVAL(cx, vp, result.optionalRetVal);  /* return undefined */
  return JS_TRUE;
}
JSBool reformer_native_getcwd(JSContext* cx, uintN argc, jsval* vp)
{
  char cwdBuffer[FILENAME_MAX];
  getcwd(cwdBuffer, sizeof(cwdBuffer));

  JSString* cwd = JS_NewStringCopyZ(cx, cwdBuffer);
  jsval rVal = STRING_TO_JSVAL(cwd);
  JS_SET_RVAL(cx, vp, rVal);
}
JSBool reformer_native_fileExists(JSContext* cx, uintN argc, jsval* vp)
{
  JSString* text;

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &text)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "reformer_native_fileExists: couldn't parse path arg");
      return JS_FALSE;
  }

  char* path = JS_EncodeString(cx, text);
  JSBool exists = JS_FALSE;
  if (fileExists(path)) {
    exists = JS_TRUE;
  }
  jsval rVal = BOOLEAN_TO_JSVAL(exists);
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

JSBool reformer_native_threadSleep(JSContext* cx, uintN argc, jsval* vp)
{
  jsuint ms;

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "u", &ms)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "reformer_native_threadSleep: couldn't parse ms arg");
      return JS_FALSE;
  }

  //printf("sleeping for %d\n", ms);
  sf::Sleep(ms);

  jsval rVal = JSVAL_VOID;
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

JSBool sugs_native_random_uuid(JSContext* cx, uintN argc, jsval* vp)
{
  char buffer[32];
  sprintf(buffer, "%d", Rng::next());
  std::string randomComponent(buffer);
	const char* randCStr = randomComponent.c_str();
	JSString* randStrObj = JS_NewStringCopyZ(cx, randCStr);
	jsval rVal = STRING_TO_JSVAL(randStrObj);
	JS_SET_RVAL(cx, vp, rVal);
	return JS_TRUE;
}

static JSFunctionSpec reformer_global_native_functions[] = {
  JS_FS("puts", reformer_native_puts, 1, 0),
  JS_FS("__native_require", reformer_native_executeScript, 2, 0),
  JS_FS("__native_require_coffee", reformer_native_executeCoffeeScript, 2, 0),
  JS_FS("__native_getcwd", reformer_native_getcwd, 0, 0),
  JS_FS("__native_fileExists", reformer_native_fileExists, 1, 0),
  JS_FS("__native_thread_sleep", reformer_native_threadSleep, 1, 0),
  JS_FS("__native_random_uuid", sugs_native_random_uuid, 0, 0),
  JS_FS_END
};

/* The class of the global object. */
static JSClass global_class = {
    "global", JSCLASS_GLOBAL_FLAGS,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

/* The error reporter callback. */
void reportError(JSContext *cx, const char *message, JSErrorReport *report)
{
  printf("!!!ERROR!!!\n");
  fprintf(stderr, "%s:%u:%s\n",
          report->filename ? report->filename : "<no filename>",
          (unsigned int) report->lineno,
          message);
}

JSRuntime* initRuntime(uint32 maxBytes) {
  JSRuntime *rt;
  rt = JS_NewRuntime(maxBytes);

  if (!rt) {
      exit(EXIT_FAILURE);
  }
  return rt;
}
jsEnv initContext(JSRuntime* rt) {
  printf("Initializing scriptmonkey javascript environment..\n");

  JSContext *cx;
  JSObject  *global;

  cx = JS_NewContext(rt, 8192);

  if (!cx) {
      exit(EXIT_FAILURE);
  }

  global = JS_NewCompartmentAndGlobalObject(cx, &global_class, NULL);
  if (global == NULL)
      exit(EXIT_FAILURE);

  if (JS_InitStandardClasses(cx, global) != JS_TRUE) {
      exit(EXIT_FAILURE);
  }

  if (!JS_DefineFunctions(cx, global, reformer_global_native_functions)) {
    printf("failure to declare functions");
    exit(EXIT_FAILURE);
  }

  JS_SetErrorReporter(cx, reportError);

#ifdef DEBUG
  JS_SetGCZeal(cx, 1, 20, JS_FALSE);
#endif

  jsEnv env = {rt, cx, global};
  return env;
}

void teardownContext(JSContext* cx) {
  //printf("Destroy JSContext...\n");
  JS_DestroyContext(cx);
}
void teardownRuntime(JSRuntime* rt)
{
  //printf("Destroy JSRuntime...\n");
  JS_DestroyRuntime(rt);
}

void shutdownSpidermonkey() {
  //printf("Shutting down SpiderMonkey...\n");
  JS_ShutDown();
}

predicateResult execStartupCallbacks(jsEnv jsEnv) {
  jsval argv[0];
  jsval rval;
  if (JS_CallFunctionName(jsEnv.cx, jsEnv.global, "doStartup", 0, argv, &rval) == JS_FALSE) {
    return {JS_FALSE, "error occured while called doStartup()\n"};
  }
  return { JS_TRUE, ""};
}

sugsConfig getCurrentConfig(JSContext* cx, JSObject* global) {
    jsval scVal;
    if(!JS_GetProperty(cx, global, "config", &scVal)) {
        printf("getCurrentConfig: failed to fetch config from global.sugsConfig\n");
        exit(EXIT_FAILURE);
    }
    JSObject* sugsConfig = JSVAL_TO_OBJECT(scVal);

    jsval widthVal;
    if(!JS_GetProperty(cx, sugsConfig, "screenWidth", &widthVal)) {
        printf("getCurrentConfig: failure to pull screenWidth from global.sugsConfig\n");
        exit(EXIT_FAILURE);
    }
    int width = SUGS_JSVAL_TO_NUMBER(widthVal);

    jsval heightVal;
    if(!JS_GetProperty(cx, sugsConfig, "screenHeight", &heightVal)) {
        printf("getCurrentConfig: failure to pull screenHeight from global.sugsConfig\n");
        exit(EXIT_FAILURE);
    }
    int height = SUGS_JSVAL_TO_NUMBER(heightVal);

    jsval colorDepthVal;
    if(!JS_GetProperty(cx, sugsConfig, "colorDepth", &colorDepthVal)) {
        printf("getCurrentConfig: failure to pull colorDepth from global.sugsConfig\n");
        exit(EXIT_FAILURE);
    }
    int colorDepth = SUGS_JSVAL_TO_NUMBER(colorDepthVal);

    jsval pathsVal;
    if(!JS_GetProperty(cx, sugsConfig, "paths", &pathsVal)) {
        printf("getCurrentConfig: failure to pull paths from global.sugsConfig\n");
        exit(EXIT_FAILURE);
    }
    JSObject* pathsObj = JSVAL_TO_OBJECT(pathsVal);

    jsuint pathsLength;
    if(!JS_GetArrayLength(cx, pathsObj, &pathsLength)) {
      printf("getCurrentConfig: failure to pull paths length from paths obj in global.sugsConfig\n");
        exit(EXIT_FAILURE);
    }

    printf("number of paths: %d\n", pathsLength);

    pathStrings paths;
    paths.length = pathsLength;
    paths.paths = new std::string[pathsLength];
    std::string cwdPrefix = getCurrentWorkingDir();
    std::string sep = "/"; // FIXME: need platform portable solution
    for(jsint ctr = 0;ctr < pathsLength;ctr++) {
      jsval entryVal;
      if (!JS_GetElement(cx, pathsObj, ctr, &entryVal)) {
        printf("getCurrentConfig: failure to pull entryVal from pathsObj.. ctr pos: %d\n", ctr);
        exit(EXIT_FAILURE);
      }
      JSString* entryStr = JSVAL_TO_STRING(entryVal);
      std::string entryString(JS_EncodeString(cx, entryStr));

      paths.paths[ctr] = cwdPrefix + sep + entryString;
      printf("stored %s as a lookup path...\n", paths.paths[ctr].c_str());
    }

    printf("done parsing out paths...\n");

    jsval moduleEntryPointVal;
    if(!JS_GetProperty(cx, sugsConfig, "moduleEntryPoint", &moduleEntryPointVal)) {
        printf("getCurrentConfig: failure to pull moduleEntryPoint from global.sugsConfig\n");
        exit(EXIT_FAILURE);
    }
    JSString* moduleEntryPointObj = JSVAL_TO_STRING(moduleEntryPointVal);
    char* moduleEntryPoint = JS_EncodeString(cx, moduleEntryPointObj);
    jsval moduleDirVal;
    if(!JS_GetProperty(cx, sugsConfig, "moduleDir", &moduleDirVal)) {
        printf("getCurrentConfig: failure to pull moduleDir from global.sugsConfig\n");
        exit(EXIT_FAILURE);
    }
    JSString* moduleDirObj = JSVAL_TO_STRING(moduleDirVal);
    char* moduleDir = JS_EncodeString(cx, moduleDirObj);

    printf("successfully parsed config...\n");
    return {
        paths,
        moduleEntryPoint,
        moduleDir,
        width,
        height,
        colorDepth,
    };
}

sugsConfig execConfig(JSContext* cx, JSObject* global)
{
  predicateResult result;
  // The introduction of user code
  if (fileExists("config.js")) {
    result = executeFullPathJavaScript("config.js", cx, global);
    if(result.result == JS_FALSE) {
      printf(result.message);
      exit(EXIT_FAILURE);
    }
  }

  return getCurrentConfig(cx, global);
}


workerInfos getWorkerInfo(JSContext* cx, JSObject* global, sugsConfig config)
{
  printf("gonna execute %s\n", config.moduleEntryPoint);

  if (fileExists(config.moduleEntryPoint)) {
    if (doesFilenameEndWithDotCoffee(config.moduleEntryPoint) == JS_TRUE) {
      printf("entry point is .coffee: %s\n", config.moduleEntryPoint);
      executeFullPathCoffeeScript(config.moduleEntryPoint, cx, global);
    }
    else {
      printf("entry point is .js\n");
      executeFullPathJavaScript(config.moduleEntryPoint, cx, global);
    }
  }
  else {
    printf("UNABLE TO FIND %s\n", config.moduleEntryPoint);
    exit(EXIT_FAILURE);
  }
  printf("after executing config...?\n");
  jsval workersVal;
  if(!JS_GetProperty(cx, global, "__workers", &workersVal)) {
    printf("getWorkerInfo: failed to pull workers out of global obj..\n");
    exit(EXIT_FAILURE);
  }
  JSObject* workersObj = JSVAL_TO_OBJECT(workersVal);

  jsval backendsVal;
  if(!JS_GetProperty(cx, workersObj, "backends", &backendsVal)) {
    printf("getWorkerInfo: failed to pull backends out of global obj..\n");
    exit(EXIT_FAILURE);
  }
  JSObject* backendsObj = JSVAL_TO_OBJECT(backendsVal);

  jsuint backendWorkersLength = 0;
  if(!JS_GetArrayLength(cx, backendsObj, &backendWorkersLength)) {
    printf("getWorkerInfo: unable to get array length of backends obj");
    exit(EXIT_FAILURE);
  }

  printf("number of backend workers: %d\n", backendWorkersLength);
  workerInfo* backendWorkers = new workerInfo[backendWorkersLength];
  for(int ctr = 0; ctr < backendWorkersLength; ctr++) {
    jsval backendScriptVal;
    if(!JS_GetElement(cx, backendsObj, ctr, &backendScriptVal)) {
      printf("can't pull element out of backendObj array\n");
      exit(EXIT_FAILURE);
    }
    JSString* backendScript = JSVAL_TO_STRING(backendScriptVal);
    workerInfo wi = {
      JS_EncodeString(cx, backendScript),
    };
    printf("BACKEND SCRIPT: %s\n", wi.entryPoint);
    backendWorkers[ctr] = wi;
  }

  jsval frontendVal;
  if(!JS_GetProperty(cx, workersObj, "frontend", &frontendVal)) {
    printf("getWorkerInfo: failed to pull frontends out of global obj..\n");
    exit(EXIT_FAILURE);
  }
  JSString* frontendStr = JSVAL_TO_STRING(frontendVal);

  workerInfo frontendWorker = {
    JS_EncodeString(cx, frontendStr),
  };

  workerInfos wi =  {
    backendWorkers,
    backendWorkersLength,
    frontendWorker
  };
  printf("FRONTEND SCRIPT: %s\n", wi.frontendWorker.entryPoint);
  return wi;
}
