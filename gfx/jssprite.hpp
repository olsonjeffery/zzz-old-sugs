#ifndef __jssprite_hpp__
#define __jssprite_hpp__

#include <jsapi.h>
#include <SFML/Graphics.hpp>

#include "../medialibrary.hpp"
#include "jsdrawable.hpp"

void registerSpriteFactory(JSContext* cx, JSObject* global);

#endif
