#include "sfmlsetup.hpp"

graphicsEnv initGraphics(JSContext* cx, sugsConfig config) {
  printf("initializing graphics environment.. conf: w:%d h:%d c:%d\n",config.screenWidth, config.screenHeight, config.colorDepth);
  sf::RenderWindow* win = new sf::RenderWindow(sf::VideoMode(config.screenWidth,
                                                             config.screenHeight,
                                                             config.colorDepth),
                                               "sugs");
  JSObject* canvas = newCanvasFrom(win, cx);

  graphicsEnv gfxEnv = { win, canvas };
  return gfxEnv;
}

void teardownGraphics(sf::RenderWindow* window, JSObject* canvas, JSContext* cx) {
  JS_RemoveObjectRoot(cx, &canvas);
  delete window;
}
