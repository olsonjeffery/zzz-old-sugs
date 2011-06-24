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
static void
classdef_canvas_finalize(JSContext* cx, JSObject* sp) {
  /*sf::RenderWindow* window = (sf::RenderWindow*)JS_GetPrivate(cx, sp);
  printf("About to try and delete an sf::RenderWindow...\n");
  delete window;*/
}
static JSClass
native_canvas_classdef = {
  "NativeCanvas",
  JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, classdef_canvas_finalize
};

static JSFunctionSpec canvas_native_functions[] = {
  JS_FS("__native_draw", reformer_native_canvas_draw, 2, 0),
  JS_FS_END
};

JSObject* newCanvasFrom(sf::RenderWindow* window, JSContext* cx) {
  JSObject* canvas = JS_NewObject(cx, &native_canvas_classdef, NULL, NULL);

  // DEFINE FUNCS FOR CANVAS!!!
  JS_DefineFunctions(cx, canvas, canvas_native_functions);

  if (!JS_SetPrivate(cx, canvas, window)) {
    printf("couldn't set private on canvas");
    exit(EXIT_FAILURE);
  }

  return canvas;
}
