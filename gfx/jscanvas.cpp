#include "jscanvas.hpp"

JSBool reformer_native_canvas_draw(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* This;
  JSObject* spriteObj;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "oo", &This, &spriteObj)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "wtf can't parse arguments for pos obj", 1);
      return JS_FALSE;
  }

  // need error check
  sf::Sprite* sprite = (sf::Sprite*)(JS_GetPrivate(cx, spriteObj));
  sf::RenderWindow* win = (sf::RenderWindow*)(JS_GetPrivate(cx, This));
  win->Draw(*sprite);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}
static JSFunctionSpec canvas_native_functions[] = {
  JS_FS("__native_draw", reformer_native_canvas_draw, 2, 0),
  JS_FS_END
};

JSObject* newCanvasFrom(sf::RenderWindow* window, JSContext* cx) {
  JSObject* canvas = JS_NewObject(cx, NULL, NULL, NULL);

  // DEFINE FUNCS FOR CANVAS!!!
  JS_DefineFunctions(cx, canvas, canvas_native_functions);

  if (!JS_SetPrivate(cx, canvas, window)) {
    printf("couldn't set private on canvas");
    exit(EXIT_FAILURE);
  }

  return canvas;
}
