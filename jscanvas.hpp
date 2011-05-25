#ifndef __jscanvas_hpp__
#define __jscanvas_hpp__

#include <jsapi.h>
#include <SFML/Graphics.hpp>

JSObject* newCanvasFrom(sf::RenderWindow* window, JSContext* cx);

#endif
