#include "jsgraphics.hpp"

/*
 *
 * PUBLIC FUNCTIONS
 *
 */

namespace sugs{
namespace richclient {
namespace gfx {

void registerGraphicsNatives(JSContext* cx, JSObject* global) {
  registerSpriteFactory(cx, global);
  registerRectangleFactory(cx, global);
  registerCircleFactory(cx, global);
  registerTextFactory(cx, global);
}

}}} // namespace sugs::richclient::gfx
