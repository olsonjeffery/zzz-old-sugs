#ifndef __jsutils_hpp__
#define __jsutils_hpp__

#include <jsapi.h>

typedef struct {
  JSRuntime* rt;
  JSContext* cx;
  JSObject* global;
} jsEnv;

#endif
