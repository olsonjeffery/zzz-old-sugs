#include "jsgraphics.hpp"

/*
 *
 * PUBLIC FUNCTIONS
 *
 */

void registerGraphicsNatives(JSContext* cx, JSObject* global) {
  registerSpriteFactory(cx, global);
  registerRectangleFactory(cx, global);
  registerCircleFactory(cx, global);
  registerTextFactory(cx, global);
}
