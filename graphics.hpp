#ifndef __graphics_hpp__
#define __graphics_hpp__

#include <SFML/Graphics.hpp>
#include <stdio.h>

typedef struct {
  sf::RenderWindow* window;
} graphicsEnv;

// graphics setup stuff
graphicsEnv initGraphics();
void teardownGraphics(sf::RenderWindow* window);

#endif
