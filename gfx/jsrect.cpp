#include "jsrect.hpp"

static JSBool
native_rectangle_factory(JSContext* cx, uintN argc, jsval* vp) {
  printf("calling native_Rectangle_factory\n");

  JSObject* rectParams;
  JSObject* colorParams;
  jsdouble lineThickness;
  JSObject* lineColorParams;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "oodo", &rectParams, &colorParams, &lineThickness, &lineColorParams)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "native_rectangle_factory: can't parse arguments");
      return JS_FALSE;
  }

  jsval widthVal;
  if(!JS_GetProperty(cx, rectParams, "width", &widthVal)) {
    JS_ReportError(cx, "native_rectangle_factory: failed to pull width val from rectParams");
    return JS_FALSE;
  }
  double width = SUGS_JSVAL_TO_NUMBER(widthVal);
  jsval heightVal;
  if(!JS_GetProperty(cx, rectParams, "height", &heightVal)) {
    JS_ReportError(cx, "native_rectangle_factory: failed to pull height val from rectParams");
    return JS_FALSE;
  }
  double height = SUGS_JSVAL_TO_NUMBER(heightVal);

  sf::Color fillColor;
  if (!getColorFrom(cx, colorParams, &fillColor)) {
    JS_ReportError(cx, "native_rectangle_factory: failed to parse out fillColor");
  }
  sf::Color lineColor;
  if (!getColorFrom(cx, lineColorParams, &lineColor)) {
    JS_ReportError(cx, "native_rectangle_factory: failed to parse out lineColor");
  }

  sf::Shape* sfmlRectangle = new sf::Shape;
  *sfmlRectangle = sf::Shape::Rectangle(0, 0, width, height, fillColor, lineThickness, lineColor);
  sfmlRectangle->SetOrigin(width / 2.f, height / 2.f);
  sfmlRectangle->SetPosition(0,0);

  JSObject* rectObj = JS_NewObject(cx, getDrawableClassDef(), NULL, NULL);
  JS_DefineFunctions(cx, rectObj, getDrawableFunctionSpec());
  JS_SetPrivate(cx, rectObj, sfmlRectangle);
  jsval rVal = OBJECT_TO_JSVAL(rectObj);

  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

void
registerRectangleFactory(JSContext* cx, JSObject* global) {
    JS_DefineFunction(cx, global, "__native_factory_rectangle", native_rectangle_factory, 1, 0);
}
