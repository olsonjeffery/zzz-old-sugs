#include "jssetup.hpp"

/* prototypes */
predicateResult executeScriptFromSrc(const char* path, char** src, int length, JSContext* cx, JSObject* global);

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

  predicateResult result = executeScript(path, cx, global);
  if (result.result == JS_FALSE) {
    JS_ReportError(cx, "reformer_native_executeScript -- error running js script '%s': %s", path, result.message);
    return JS_FALSE;
  }

  JS_SET_RVAL(cx, vp, JSVAL_VOID);  /* return undefined */
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

  predicateResult result = executeCoffeeScript(path, cx, global);
  if (result.result == JS_FALSE) {
    JS_ReportError(cx, "reformer_native_executeCoffeeScript -- error running coffee script '%s': %s", path, result.message);
    return JS_FALSE;
  }

  JS_SET_RVAL(cx, vp, JSVAL_VOID);  /* return undefined */
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

static JSFunctionSpec reformer_global_native_functions[] = {
  JS_FS("puts", reformer_native_puts, 1, 0),
  JS_FS("__native_load", reformer_native_executeScript, 2, 0),
  JS_FS("__native_load_coffee", reformer_native_executeCoffeeScript, 2, 0),
  JS_FS("__native_getcwd", reformer_native_getcwd, 0, 0),
  JS_FS("__native_fileExists", reformer_native_fileExists, 1, 0),
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

jsEnv initJsEnvironment() {
  printf("Initializing scriptmonkey javascript environment..\n");

  JSContext *cx;
  JSRuntime *rt;
  JSObject  *global;

  rt = JS_NewRuntime(128L * 1024L * 1024L);

  if (!rt) {
      exit(EXIT_FAILURE);
  }

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
  registerGraphicsNatives(cx, global);
  registerInputNatives(cx, global);

  JS_SetErrorReporter(cx, reportError);

  jsEnv env = {rt, cx, global};
  return env;
}

predicateResult executeScript(const char* path, JSContext* cx, JSObject* global) {
  char* srcBuffer;
  int length;
  if (!fileExists(path)) {
    char msg[2056];
    sprintf(msg, "Script %s not found!\n", path);
    return {JS_FALSE, msg};
  }
  readEntireFile(path, &srcBuffer, &length);

  predicateResult result = executeScriptFromSrc(path, &srcBuffer, length, cx, global);
  free(srcBuffer);
  return result;
}

predicateResult executeCoffeeScript(const char* path, JSContext* cx, JSObject* global) {
  char* buffer;
  int length;
  if (!fileExists(path)) {
    char msg[2056];
    sprintf(msg, "Script %s not found and can't be loaded!\n", path);
    return {JS_FALSE, msg};
  }
  readEntireFile(path, &buffer, &length);

  jsval argv[2];
  JSString* coffeeFileText = JS_NewStringCopyZ(cx, buffer);
  argv[0] = STRING_TO_JSVAL(coffeeFileText);
  JSObject* options = JS_NewObject(cx, NULL, NULL, NULL);
  argv[1] = OBJECT_TO_JSVAL(options);

  jsval csVal;
  if (JS_GetProperty(cx, global, "CoffeeScript", &csVal) != JS_TRUE) {
    return { JS_FALSE, "couldn't get CoffeeScript prop from global\n" };
  }

  JSObject* coffeeScript = JSVAL_TO_OBJECT(csVal);

  JSBool hasCompile;
  JS_HasProperty(cx, coffeeScript, "compile", &hasCompile);
  if (hasCompile != JS_TRUE) {
    printf("barf time?\n", path);
    return {JS_FALSE, "no compile function on CoffeeScript obj. grabbed wrong one?\n"};
  }

  jsval rVal;
  if (JS_CallFunctionName(cx, coffeeScript, "compile", 2, argv, &rVal) != JS_TRUE) {
    char buffer[2056];
    sprintf(buffer, "Failed to compile coffeescript file %s\n", path);
    return {JS_FALSE, buffer};
  }

  JSString* jsSrcString;
  jsSrcString = JSVAL_TO_STRING(rVal);

  char* srcString = JS_EncodeString(cx, jsSrcString);
  if (srcString == NULL) {
    return { JS_FALSE, "failed to encode string when prepping to run coffee file.\n"};
  }

  predicateResult result = executeScriptFromSrc(path, &srcString, strlen(srcString) - sizeof(char), cx, global);
  free(buffer);
  return result;
}

predicateResult executeScriptFromSrc(const char* path, char** src, int length, JSContext* cx, JSObject* global) {
  printf("executing %s from source...\n", path);
  /* Execute a script */
  JSObject *scriptObject;
  jsval rval;

  /* Compile a script file into a script object */
  scriptObject = JS_CompileScript(cx, global, *src, length, path, 1);
  if (!scriptObject) {
    char buffer[2056];
    sprintf(buffer, "Failed to compile %s\n", path);
    return {JS_FALSE, buffer};
  }

  if (JS_AddObjectRoot(cx, &scriptObject) != JS_TRUE) {
    char buffer[2056];
    sprintf(buffer, "Failed to add root object for %s\n", path);
    return {JS_FALSE, buffer};
  }

  /* Execute script object */
  if (JS_ExecuteScript(cx, global, scriptObject, &rval) != JS_TRUE) {
    char buffer[2056];
    sprintf(buffer, "Failed to execute %s\n", path);
    return {JS_FALSE, buffer};
  }

  // done with script..
  if (JS_RemoveObjectRoot(cx, &scriptObject) != JS_TRUE) {
    char buffer[2056];
    sprintf(buffer, "Failed to remove root object for %s\n", path);
    return {JS_FALSE, buffer};
  }

  printf("done executing %s from source...\n", path);
  return {JS_TRUE, ""};
}

void teardownJsEnvironment(JSRuntime* rt, JSContext* cx)
{
  printf("Destroy context...\n");
  JS_DestroyContext(cx);
  printf("Destroy runtime...\n");
  JS_DestroyRuntime(rt);
  printf("js shutdown...\n");
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
    if(!JS_GetProperty(cx, global, "sugsConfig", &scVal)) {
        printf("getCurrentConfig: failed to fetch config from global.sugsConfig\n");
        exit(EXIT_FAILURE);
    }
    JSObject* sugsConfig = JSVAL_TO_OBJECT(scVal);

    jsval widthVal;
    if(!JS_GetProperty(cx, sugsConfig, "screenWidth", &widthVal)) {
        printf("getCurrentConfig: failure to pull screenWidth from global.sugsConfig\n");
        exit(EXIT_FAILURE);
    }
    double width = SUGS_JSVAL_TO_NUMBER(widthVal);

    jsval heightVal;
    if(!JS_GetProperty(cx, sugsConfig, "screenHeight", &heightVal)) {
        printf("getCurrentConfig: failure to pull screenHeight from global.sugsConfig\n");
        exit(EXIT_FAILURE);
    }
    double height = SUGS_JSVAL_TO_NUMBER(heightVal);

    jsval colorDepthVal;
    if(!JS_GetProperty(cx, sugsConfig, "colorDepth", &colorDepthVal)) {
        printf("getCurrentConfig: failure to pull colorDepth from global.sugsConfig\n");
        exit(EXIT_FAILURE);
    }
    double colorDepth = SUGS_JSVAL_TO_NUMBER(colorDepthVal);

    jsval moduleDirVal;
    if(!JS_GetProperty(cx, sugsConfig, "moduleDir", &moduleDirVal)) {
        printf("getCurrentConfig: failure to pull moduleDir from global.sugsConfig\n");
        exit(EXIT_FAILURE);
    }
    JSString* moduleDirObj = JSVAL_TO_STRING(moduleDirVal);
    char* moduleDir = JS_EncodeString(cx, moduleDirObj);

    jsval moduleEntryPointVal;
    if(!JS_GetProperty(cx, sugsConfig, "moduleEntryPoint", &moduleEntryPointVal)) {
        printf("getCurrentConfig: failure to pull moduleEntryPoint from global.sugsConfig\n");
        exit(EXIT_FAILURE);
    }
    JSString* moduleEntryPointObj = JSVAL_TO_STRING(moduleEntryPointVal);
    char* moduleEntryPoint = JS_EncodeString(cx, moduleEntryPointObj);

    jsval isCoffeeVal;
    if(!JS_GetProperty(cx, sugsConfig, "entryPointIsCoffee", &isCoffeeVal)) {
        printf("couldn't get isCoffee from config\n");
        exit(EXIT_FAILURE);
    }
    JSBool entryPointIsCoffee = JSVAL_TO_BOOLEAN(isCoffeeVal);

    return {
        moduleDir,
        moduleEntryPoint,
        width,
        height,
        colorDepth,
        entryPointIsCoffee
    };
}

sugsConfig execConfig(jsEnv jsEnv)
{
  predicateResult result;
  // The introduction of user code
  if (fileExists("config.js")) {
    result = executeScript("config.js", jsEnv.cx, jsEnv.global);
    if(result.result == JS_FALSE) {
      printf(result.message);
      exit(EXIT_FAILURE);
    }
  }
  else {
    result = executeCoffeeScript("config.coffee", jsEnv.cx, jsEnv.global);
    if(result.result == JS_FALSE) {
      printf(result.message);
      exit(EXIT_FAILURE);
    }
  }

  return getCurrentConfig(jsEnv.cx, jsEnv.global);
}
