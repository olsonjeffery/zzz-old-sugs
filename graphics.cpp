#include "graphics.hpp"

graphicsEnv initGraphics() {
  printf("initializing graphics environment");
  sf::RenderWindow* win = new sf::RenderWindow(sf::VideoMode(640,480, 32), "reFormer");

  graphicsEnv gfxEnv = { win };
  return gfxEnv;
}

void teardownGraphics(sf::RenderWindow* window) {
  delete window;
}
