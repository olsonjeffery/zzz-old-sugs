#ifndef __jssetup_hpp__
#define __jssetup_hpp__

#define XP_UNIX
#include <jsapi.h>
#include <stdlib.h>
#include <stdio.h>

#include "jsutils.hpp"
#include "jsgraphics.hpp"


jsEnv initJsEnvironment();
void teardownJsEnvironment(JSRuntime* rt, JSContext* cx);
void executeScript(const char* path, JSContext* cx, JSObject* global);

#endif
