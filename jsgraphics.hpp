#ifndef __jsgraphics_hpp__
#define __jsgraphics_hpp__

#include <jsapi.h>

#include "graphics.hpp"
#include "medialibrary.hpp"
#include "jsutils.hpp"

void callIntoJsRender(jsEnv jsEnv, graphicsEnv gfxEnv);

void registerGraphicsNatives(JSContext* cx, JSObject* global);

#endif
