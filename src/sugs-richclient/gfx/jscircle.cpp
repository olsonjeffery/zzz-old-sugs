/* Copyright 2011 Jeffery Olson <olson.jeffery@gmail.com>. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this list of
 *     conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice, this list
 *     of conditions and the following disclaimer in the documentation and/or other materials
 *     provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY JEFFERY OLSON <OLSON.JEFFERY@GMAIL.COM> ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * JEFFERY OLSON <OLSON.JEFFERY@GMAIL.COM> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of Jeffery Olson <olson.jeffery@gmail.com>.
 *
 */

#include "jsrect.hpp"

static JSBool
native_circle_factory(JSContext* cx, uintN argc, jsval* vp) {

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
