#ifndef __jsgraphics_hpp__
#define __jsgraphics_hpp__

#include <math.h>
#include <jsapi.h>

#include "../medialibrary.hpp"
#include "../common.hpp"
#include "jssprite.hpp"

void callIntoJsRender(jsEnv jsEnv, graphicsEnv gfxEnv, eventEnv eventEnv, double frametime, double framerate);

void registerGraphicsNatives(JSContext* cx, JSObject* global);

#endif
