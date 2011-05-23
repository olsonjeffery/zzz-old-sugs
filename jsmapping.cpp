#include "jsmapping.hpp"


JSBool reformer_native_puts(JSContext* cx, uintN argc, jsval* vp)
{
  printf("Hello world from javascript!\n");
  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSFunctionSpec reformer_native_functions[] = {
  JS_FS("puts", reformer_native_puts, 0, 0),
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
  printf("Initializing scriptmonkey javascript environment..?\n");

  JSContext *cx;
  JSRuntime *rt;
  JSObject  *global;

  printf("before creating runtime..\n");
  /* Create an instance of the engine */
  rt = JS_NewRuntime(1024*1024);


  if (!rt) {
      exit(EXIT_FAILURE);
  }

  printf("created runtime..\n");

  /* Create an execution context */
  cx = JS_NewContext(rt, 8192);

  if (!cx) {
      exit(EXIT_FAILURE);
  }

  printf("created context...\n");

  /* Create the global object in a new compartment. */
  global = JS_NewCompartmentAndGlobalObject(cx, &global_class, NULL);
  if (global == NULL)
      exit(EXIT_FAILURE);

  printf("created global obj\n");

  if (JS_InitStandardClasses(cx, global) != JS_TRUE) {
      exit(EXIT_FAILURE);
  }

  /* Define functions */
  if (!JS_DefineFunctions(cx, global, reformer_native_functions)) {
    printf("failure to declare functions");
    exit(EXIT_FAILURE);
  }

  jsEnv env = {rt, cx, global};
  return env;
}

void executeScript(const char* path, JSContext* cx, JSObject* global)
{
  printf("preparing to run script ");
  printf(path);
  printf("\n");
  /* Execute a script */
  JSObject *scriptObject;
  jsval rval;

  /* Compile a script file into a script object */
  scriptObject = JS_CompileFile(cx, global, path);
  if (!scriptObject) {
      exit(EXIT_FAILURE);
  }

  if (JS_AddObjectRoot(cx, &scriptObject) != JS_TRUE) {
    exit(EXIT_FAILURE);
  }

  /* Execute script object */
  if (JS_ExecuteScript(cx, global, scriptObject, &rval) != JS_TRUE) {
      exit(EXIT_FAILURE);
  }

  // done with script..
  if (JS_RemoveObjectRoot(cx, &scriptObject) != JS_TRUE) {
    exit(EXIT_FAILURE);
  }
}

void teardownJsEnvironment(JSRuntime* rt, JSContext* cx)
{
  printf("Tearing down javascript environment...\n");
  /* Cleanup */
  JS_DestroyContext(cx);
  JS_DestroyRuntime(rt);
  JS_ShutDown();
}
