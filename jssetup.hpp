#ifndef __jssetup_hpp__
#define __jssetup_hpp__

#define XP_UNIX
#include <jsapi.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "types.hpp"
#include "jsgraphics.hpp"
#include "jsinput.hpp"


jsEnv initJsEnvironment();
void teardownJsEnvironment(JSRuntime* rt, JSContext* cx);
predicateResult executeScript(const char* path, JSContext* cx, JSObject* global);
predicateResult executeCoffeeScript(const char* path, JSContext* cx, JSObject* global);
predicateResult execStartupCallbacks(jsEnv jsEnv);

#endif
