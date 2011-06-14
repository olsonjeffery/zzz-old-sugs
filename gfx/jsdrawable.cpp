#include "jsdrawable.hpp"

JSBool reformer_native_drawable_setPosition(JSContext* cx, uintN argc, jsval* vp)
{
  // note that, here, we're basically saying that this native function
  // is only meant to be used as a method, because we have a This.. currently
  // requires some legwork to get the enclosing object.. until I can find a better
  // way..
  JSObject* This;
  JSObject* posObj;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "oo", &This, &posObj)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "wtf can't parse arguments for pos obj", 1);
      return JS_FALSE;
  }
  jsval xVal;
  jsval yVal;
  JS_GetProperty(cx, posObj, "x", &xVal);
  JS_GetProperty(cx, posObj, "y", &yVal);
  int x = JSVAL_TO_INT(xVal);
  int y = JSVAL_TO_INT(yVal);

  sf::Drawable* drawable = (sf::Drawable*)(JS_GetPrivate(cx, This));
  drawable->SetPosition(x, y);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}
JSBool reformer_native_drawable_getPosition(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* This;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "o", &This)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "wtf can't parse arguments for getting pos obj", 1);
      return JS_FALSE;
  }
  sf::Drawable* drawable = (sf::Drawable*)(JS_GetPrivate(cx, This));
  sf::Vector2f pos = drawable->GetPosition();
  jsval x;
  JS_NewNumberValue(cx, pos.x, &x);
  jsval y;
  JS_NewNumberValue(cx, pos.y, &y);

  JSObject* posObj = JS_NewObject(cx, NULL, NULL, NULL);
  JS_SetProperty(cx, posObj, "x", &x);
  JS_SetProperty(cx, posObj, "y", &y);

  jsval rVal = OBJECT_TO_JSVAL(posObj);
  JS_SET_RVAL(cx, vp, rVal);

  return JS_TRUE;
}
JSBool reformer_native_drawable_getRotation(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* This;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "o", &This)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "reformer_native_drawable_getRotation: can't parse args", 1);
      return JS_FALSE;
  }
  sf::Drawable* drawable = (sf::Drawable*)(JS_GetPrivate(cx, This));
  jsval rotVal = DOUBLE_TO_JSVAL(drawable->GetRotation());

  JS_SET_RVAL(cx, vp, rotVal);

  return JS_TRUE;
}
JSBool reformer_native_drawable_move(JSContext* cx, uintN argc, jsval* vp)
{
  // note that, here, we're basically saying that this native function
  // is only meant to be used as a method, because we have a This.. currently
  // requires some legwork to get the enclosing object.. until I can find a better
  // way..
  JSObject* This;
  JSObject* posObj;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "oo", &This, &posObj)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "reformer_native_drawable_move: can't parse out arguments", 1);
      return JS_FALSE;
  }
  jsval xVal;
  jsval yVal;
  JS_GetProperty(cx, posObj, "x", &xVal);
  JS_GetProperty(cx, posObj, "y", &yVal);
  double x = JSVAL_TO_DOUBLE(xVal);
  if (isnan(x)) {
    x = 0.f;
  }
  double y = JSVAL_TO_DOUBLE(yVal);
  if (isnan(y)) {
    y = 0.f;
  }

  sf::Drawable* drawable = (sf::Drawable*)(JS_GetPrivate(cx, This));
  drawable->Move(x, y);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}
static JSBool
reformer_native_drawable_rotate(JSContext* cx, uintN argc, jsval* vp)
{
  // note that, here, we're basically saying that this native function
  // is only meant to be used as a method, because we have a This.. currently
  // requires some legwork to get the enclosing object.. until I can find a better
  // way..
  JSObject* This;
  jsdouble rotDegree;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "od", &This, &rotDegree)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "reformer_native_drawable_rotate: can't parse out arguments", 1);
      return JS_FALSE;
  }
  sf::Drawable* drawable = (sf::Drawable*)(JS_GetPrivate(cx, This));
  drawable->Rotate(rotDegree);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}
static JSFunctionSpec
drawable_native_functions[] = {
  JS_FS("__native_setPos", reformer_native_drawable_setPosition, 2, 0),
  JS_FS("__native_getPos", reformer_native_drawable_getPosition, 1, 0),
  JS_FS("__native_getRotation", reformer_native_drawable_getRotation, 1, 0),
  JS_FS("__native_move", reformer_native_drawable_move, 2, 0),
  JS_FS("__native_rotate", reformer_native_drawable_rotate, 2, 0),
  JS_FS_END
};

static void
classdef_drawable_finalize(JSContext* cx, JSObject* sp) {
  sf::Drawable* sprite = (sf::Drawable*)JS_GetPrivate(cx, sp);
  printf("About to try and delete an sf::Drawable...");
  delete sprite;
}
static JSClass
drawableClassDef = {
  "NativeDrawable",
  JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, classdef_drawable_finalize
};

JSFunctionSpec*
getDrawableFunctionSpec() {
    return drawable_native_functions;
}

JSClass*
getDrawableClassDef() {
    return &drawableClassDef;
}
