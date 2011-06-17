#ifndef __jstext_hpp__
#define __jstext_hpp__

#include <jsapi.h>
#include <SFML/Graphics.hpp>

#include "../common.hpp"
#include "jsdrawable.hpp"
#include "../medialibrary.hpp"

void registerTextFactory(JSContext* cx, JSObject* global);

#endif
