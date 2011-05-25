#include "graphics.hpp"

graphicsEnv initGraphics(JSContext* cx) {
  printf("initializing graphics environment");
  sf::RenderWindow* win = new sf::RenderWindow(sf::VideoMode(640,480, 32), "reFormer");
  JSObject* canvas = newCanvasFrom(win, cx);

  graphicsEnv gfxEnv = { win, canvas };
  return gfxEnv;
}

void teardownGraphics(sf::RenderWindow* window, JSObject* canvas, JSContext* cx) {
  JS_RemoveObjectRoot(cx, &canvas);
  delete window;
}
