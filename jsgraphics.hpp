#ifndef __jsgraphics_hpp__
#define __jsgraphics_hpp__

#include <jsapi.h>

#include "medialibrary.hpp"
#include "common.hpp"

void callIntoJsRender(jsEnv jsEnv, graphicsEnv gfxEnv);

void registerGraphicsNatives(JSContext* cx, JSObject* global);

#endif
