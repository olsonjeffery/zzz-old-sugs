#define XP_UNIX
#include <jsapi.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
  JSRuntime* rt;
  JSContext* cx;
  JSObject* global;
} jsEnv;

jsEnv initJsEnvironment();
void teardownJsEnvironment(JSRuntime* rt, JSContext* cx);
void executeScript(const char* path);
