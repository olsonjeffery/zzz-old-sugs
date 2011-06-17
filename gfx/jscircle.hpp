#ifndef __jscircle_hpp__
#define __jscircle_hpp__

#include <jsapi.h>
#include <SFML/Graphics.hpp>

#include "../common.hpp"
#include "jsdrawable.hpp"

void registerCircleFactory(JSContext* cx, JSObject* global);

#endif
