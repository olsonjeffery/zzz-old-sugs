#ifndef __sfmlsetup_hpp__
#define __sfmlsetup_hpp__

#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <jsapi.h>

#include "../common.hpp"
#include "jscanvas.hpp"

// graphics setup stuff
graphicsEnv initGraphics(JSContext* cx);
void teardownGraphics(sf::RenderWindow* window, JSObject* canvas, JSContext* cx);

#endif
