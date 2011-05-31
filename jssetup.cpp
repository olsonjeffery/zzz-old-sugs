#include "jssetup.hpp"

/* NATIVE UTIL FUNCTIONS */
JSBool reformer_native_puts(JSContext* cx, uintN argc, jsval* vp)
{
  JSString* text;

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &text)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "wtf can't parse arguments", 1);
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
      JS_ReportError(cx, "wtf can't parse arguments", 1);
      return JS_FALSE;
  }

  char* path = JS_EncodeString(cx, text);

  predicateResult result = executeScript(path, cx, global);
  if (result.result == JS_FALSE) {
    JS_ReportError(cx, "wtf can't parse arguments", 1);
    return JS_FALSE;
  }

  JS_SET_RVAL(cx, vp, JSVAL_VOID);  /* return undefined */
  return JS_TRUE;
}

static JSFunctionSpec reformer_global_native_functions[] = {
  JS_FS("puts", reformer_native_puts, 1, 0),
  JS_FS("__native_load", reformer_native_executeScript, 2, 0),
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

  //printf("before creating runtime..\n");
  /* Create an instance of the engine */
  rt = JS_NewRuntime(1024*1024);

  if (!rt) {
      exit(EXIT_FAILURE);
  }

  //printf("created runtime..\n");

  /* Create an execution context */
  cx = JS_NewContext(rt, 8192);

  if (!cx) {
      exit(EXIT_FAILURE);
  }

  //printf("created context...\n");

  /* Create the global object in a new compartment. */
  global = JS_NewCompartmentAndGlobalObject(cx, &global_class, NULL);
  if (global == NULL)
      exit(EXIT_FAILURE);

  //printf("created global obj\n");

  if (JS_InitStandardClasses(cx, global) != JS_TRUE) {
      exit(EXIT_FAILURE);
  }

  /* Define functions */
  if (!JS_DefineFunctions(cx, global, reformer_global_native_functions)) {
    printf("failure to declare functions");
    exit(EXIT_FAILURE);
  }
  registerGraphicsNatives(cx, global);

  jsEnv env = {rt, cx, global};
  return env;
}

predicateResult runConfig(JSContext* cx, JSObject *global, char* configPath) {
  executeScript(configPath, cx, global);
}

predicateResult executeScript(const char* path, JSContext* cx, JSObject* global)
{
  printf("executing %s...\n", path);
  /* Execute a script */
  JSObject *scriptObject;
  jsval rval;

  /* Compile a script file into a script object */
  scriptObject = JS_CompileFile(cx, global, path);
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

  return {JS_TRUE, ""};
}

void teardownJsEnvironment(JSRuntime* rt, JSContext* cx)
{
  printf("Tearing down javascript environment...\n");
  /* Cleanup */
  JS_DestroyContext(cx);
  JS_DestroyRuntime(rt);
  JS_ShutDown();
}

predicateResult execStartupCallbacks(jsEnv jsEnv) {
  jsval argv[0];
  jsval rval;
  if (JS_CallFunctionName(jsEnv.cx, jsEnv.global, "doStartup", 0, argv, &rval) == JS_FALSE) {
    return {JS_FALSE, "error occured while called doStartup()"};
  }
  return { JS_TRUE, ""};
}
