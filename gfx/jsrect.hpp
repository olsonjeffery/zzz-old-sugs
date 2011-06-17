#ifndef __jsrect_hpp__
#define __jsrect_hpp__

#include <jsapi.h>
#include <SFML/Graphics.hpp>

#include "../common.hpp"
#include "jsdrawable.hpp"

void registerRectangleFactory(JSContext* cx, JSObject* global);

#endif
