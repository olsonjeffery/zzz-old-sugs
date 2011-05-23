#include "jsmapping.hpp"

  static void
  hello_world(void)
  {
    printf("Hello world from javascript!\n");
  }

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

  jsEnv env = {rt, cx, global};
  return env;
}

void executeScript(const char* path)
{
  /* Execute a script */
  /*JSScript *script;
  jsval rval;
  */
  /* Compile a script file into a script object */
  /*script = JS_CompileFile(cx, global, path);
  if (!script) {
      exit(EXIT_FAILURE);
  }*/

  /* Execute script object */
  /*if (JS_ExecuteScript(cx, global, script, &rval) != JS_TRUE) {
      exit(EXIT_FAILURE);
  }*/

  /* Remove script object from memory */
  /*JS_DestroyScript(cx, script);
  */
}

void teardownJsEnvironment(JSRuntime* rt, JSContext* cx)
{
  printf("Tearing down javascript environment...\n");
  /* Cleanup */
  JS_DestroyContext(cx);
  JS_DestroyRuntime(rt);
  JS_ShutDown();
}
