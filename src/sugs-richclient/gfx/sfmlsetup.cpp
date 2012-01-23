#include "gfx.hpp"

namespace sugs {
namespace richclient {
namespace gfx {

GraphicsEnv initGraphics(JSContext* cx, int width, int height, int colorDepth) {
  printf("initializing graphics environment.. conf: w:%d h:%d c:%d\n",width, height, colorDepth);
  sf::RenderWindow* win = new sf::RenderWindow(sf::VideoMode(width,
                                                             height,
                                                             colorDepth),
                                               "sugs");
  JSObject* canvas = newCanvasFrom(win, cx);

  GraphicsEnv gfxEnv = { win, canvas };
  return gfxEnv;
}

void teardownGraphics(sf::RenderWindow* window, JSObject* canvas, JSContext* cx) {
  JS_RemoveObjectRoot(cx, &canvas);
  delete window;
}

}}}
