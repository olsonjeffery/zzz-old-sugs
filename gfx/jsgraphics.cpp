#include "jsgraphics.hpp"

/*
 *
 * PUBLIC FUNCTIONS
 *
 */
void callIntoJsRender(jsEnv jsEnv, graphicsEnv gfxEnv, eventEnv evEnv, double frametime, double framerate) {
  jsval argv[4];

  argv[0] = OBJECT_TO_JSVAL(gfxEnv.canvas);
  argv[1] = OBJECT_TO_JSVAL(evEnv.input);
  argv[2] = DOUBLE_TO_JSVAL(frametime);
  argv[3] = DOUBLE_TO_JSVAL(framerate);
  jsval rval;
  JS_CallFunctionName(jsEnv.cx, jsEnv.global, "renderSprites", 4, argv, &rval);
}

void registerGraphicsNatives(JSContext* cx, JSObject* global) {
  registerSpriteFactory(cx, global);
}

