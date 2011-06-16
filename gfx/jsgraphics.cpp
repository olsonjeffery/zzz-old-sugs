#include "jsgraphics.hpp"

/*
 *
 * PUBLIC FUNCTIONS
 *
 */
void callIntoJsRender(jsEnv jsEnv, graphicsEnv gfxEnv, eventEnv evEnv, int msElapsed) {
  jsval argv[3];

  argv[0] = OBJECT_TO_JSVAL(gfxEnv.canvas);
  argv[1] = OBJECT_TO_JSVAL(evEnv.input);
  argv[2] = INT_TO_JSVAL(msElapsed);
  jsval rval;
  JS_CallFunctionName(jsEnv.cx, jsEnv.global, "renderSprites", 3, argv, &rval);
}

void registerGraphicsNatives(JSContext* cx, JSObject* global) {
  registerSpriteFactory(cx, global);
}

