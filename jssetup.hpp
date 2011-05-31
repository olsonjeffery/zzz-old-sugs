#ifndef __jssetup_hpp__
#define __jssetup_hpp__

#define XP_UNIX
#include <jsapi.h>
#include <stdlib.h>
#include <stdio.h>

#include "types.hpp"
#include "jsgraphics.hpp"


jsEnv initJsEnvironment();
void teardownJsEnvironment(JSRuntime* rt, JSContext* cx);
predicateResult executeScript(const char* path, JSContext* cx, JSObject* global);

#endif
