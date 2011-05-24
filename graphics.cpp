#include "graphics.hpp"

graphicsEnv initGraphics() {
  sf::RenderWindow* win = new sf::RenderWindow(sf::VideoMode(640,480, 32), "reFormer");

  graphicsEnv env = { win };
  return env;
}
