#ifndef __jsinput_hpp__
#define __jsinput_hpp__

#include <SFML/Graphics.hpp>
#include <jsapi.h>

JSObject* newInputFrom(sf::RenderWindow* window, JSContext* cx);

void registerInputNatives(JSContext* cx, JSObject* global);
#endif
