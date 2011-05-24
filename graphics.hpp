#include <SFML/Graphics.hpp>
#include <stdio.h>

typedef struct {
  sf::RenderWindow* window;
} graphicsEnv;

// graphics setup stuff
graphicsEnv initGraphics();
void teardownGraphics(sf::RenderWindow* window);

// image library
sf::Image* loadImage(char* path);
