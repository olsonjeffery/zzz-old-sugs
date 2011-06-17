#include "jsrect.hpp"

static JSBool
native_circle_factory(JSContext* cx, uintN argc, jsval* vp) {
  printf("calling native_Circle_factory\n");

  jsdouble radius;
  JSObject* colorParams;
  jsdouble lineThickness;
  JSObject* lineColorParams;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "dodo", &radius, &colorParams, &lineThickness, &lineColorParams)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "native_Circle_factory: can't parse arguments");
      return JS_FALSE;
  }

  sf::Color fillColor;
  if (!getColorFrom(cx, colorParams, &fillColor)) {
    JS_ReportError(cx, "native_Circle_factory: failed to parse out fillColor");
    return JS_FALSE;
  }
  sf::Color lineColor;
  if (!getColorFrom(cx, lineColorParams, &lineColor)) {
    JS_ReportError(cx, "native_Circle_factory: failed to parse out lineColor");
    return JS_FALSE;
  }

  sf::Shape* sfmlCircle = new sf::Shape;
  *sfmlCircle = sf::Shape::Circle(0, 0, radius, fillColor, lineThickness, lineColor);
  sfmlCircle->SetPosition(0,0);

  JSObject* circleObj = JS_NewObject(cx, getDrawableClassDef(), NULL, NULL);
  JS_DefineFunctions(cx, circleObj, getDrawableFunctionSpec());
  JS_SetPrivate(cx, circleObj, sfmlCircle);
  jsval rVal = OBJECT_TO_JSVAL(circleObj);

  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

void
registerCircleFactory(JSContext* cx, JSObject* global) {
    JS_DefineFunction(cx, global, "__native_factory_circle", native_circle_factory, 4, 0);
}
