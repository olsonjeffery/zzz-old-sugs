#include "graphics.hpp"

graphicsEnv initGraphics() {
  sf::RenderWindow* win = new sf::RenderWindow(sf::VideoMode(640,480, 32), "reFormer");

  graphicsEnv gfxEnv = { win };
  return gfxEnv;
}

void teardownGraphics(sf::RenderWindow* window) {
  delete window;
}
