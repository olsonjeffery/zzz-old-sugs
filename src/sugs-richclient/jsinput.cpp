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

#include "jsinput.hpp"

JSObject* getKeyObjectFor(JSContext* cx, sf::Key::Code keyCode);
JSObject* newKeysFinderObject(JSContext* cx);

// KeyFinder funcs
static JSBool
classdef_keyfinder_resolver(JSContext *cx, JSObject *obj, jsid id) {
  jsval idVal;
  if (JS_IdToValue(cx, id, &idVal) != JS_TRUE) {
    JS_ReportError(cx, "Unable to convert jsid to jsval");
    return JS_FALSE;
  }

  JSString* propNameJsString = JSVAL_TO_STRING(idVal);
  char* propName = JS_EncodeString(cx, propNameJsString);

  JSObject* keyObj = 0;
  if (strcmp(propName, "Up") == 0) {
    keyObj = getKeyObjectFor(cx, sf::Key::Up);
  }
  else if (strcmp(propName, "Down") == 0) {
    keyObj = getKeyObjectFor(cx, sf::Key::Down);
  }
  else if (strcmp(propName, "Left") == 0) {
    keyObj = getKeyObjectFor(cx, sf::Key::Left);
  }
  else if (strcmp(propName, "Right") == 0) {
    keyObj = getKeyObjectFor(cx, sf::Key::Right);
  }
  else if (strcmp(propName, "Num1") == 0) {
    keyObj = getKeyObjectFor(cx, sf::Key::Num1);
  }
  else if (strcmp(propName, "Num2") == 0) {
    keyObj = getKeyObjectFor(cx, sf::Key::Num2);
  }
  else if (strcmp(propName, "Num3") == 0) {
    keyObj = getKeyObjectFor(cx, sf::Key::Num3);
  }
  else if (strcmp(propName, "Num4") == 0) {
    keyObj = getKeyObjectFor(cx, sf::Key::Num4);
  }
  else if (strcmp(propName, "Num5") == 0) {
    keyObj = getKeyObjectFor(cx, sf::Key::Num5);
  }
  else if (strcmp(propName, "Num6") == 0) {
    keyObj = getKeyObjectFor(cx, sf::Key::Num6);
  }
  else if (strcmp(propName, "Num7") == 0) {
    keyObj = getKeyObjectFor(cx, sf::Key::Num7);
  }
  else if (strcmp(propName, "Num8") == 0) {
    keyObj = getKeyObjectFor(cx, sf::Key::Num8);
  }
  else if (strcmp(propName, "Num9") == 0) {
    keyObj = getKeyObjectFor(cx, sf::Key::Num9);
  }
  else if (strcmp(propName, "Num0") == 0) {
    keyObj = getKeyObjectFor(cx, sf::Key::Num0);
  }
  else if (strcmp(propName, "W") == 0) {
    keyObj = getKeyObjectFor(cx, sf::Key::W);
  }
  else if (strcmp(propName, "S") == 0) {
    keyObj = getKeyObjectFor(cx, sf::Key::S);
  }
  else if (strcmp(propName, "X") == 0) {
    keyObj = getKeyObjectFor(cx, sf::Key::X);
  }
  else if (strcmp(propName, "Space") == 0) {
    keyObj = getKeyObjectFor(cx, sf::Key::Space);
  }
  else if (strcmp(propName, "Escape") == 0) {
    keyObj = getKeyObjectFor(cx, sf::Key::Escape);
  }
  else {
    JS_ReportError(cx, "Unable to find matching key for %s", propName);
    return JS_FALSE;
  }

  if (keyObj != 0) {
    jsval propVal = OBJECT_TO_JSVAL(keyObj);
    JS_SetProperty(cx, obj, propName, &propVal);
  }

  return JS_TRUE;
}

static void
classdef_keyobject_finalize(JSContext* cx, JSObject* kf) {
  sf::Key::Code* keyCode = (sf::Key::Code*)JS_GetPrivate(cx, kf);
  delete keyCode;
}

static JSClass keyObjectClassDef = {
  "NativeKeyObject",
  JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, classdef_keyobject_finalize
};

JSBool keyObj_native_toString(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* keyObj = JS_THIS_OBJECT(cx, vp);
  sf::Key::Code keyCode = *((sf::Key::Code*)JS_GetPrivate(cx, keyObj));
  JSString* retString;

  switch(keyCode) {
    case sf::Key::Up:
      retString = JS_NewStringCopyZ(cx, "Up");
      break;
    case sf::Key::Down:
      retString = JS_NewStringCopyZ(cx, "Down");
      break;
    case sf::Key::Left:
      retString = JS_NewStringCopyZ(cx, "Left");
      break;
    case sf::Key::Right:
      retString = JS_NewStringCopyZ(cx, "Right");
      break;
    case sf::Key::Num1:
      retString = JS_NewStringCopyZ(cx, "Num1");
      break;
    case sf::Key::Num2:
      retString = JS_NewStringCopyZ(cx, "Num2");
      break;
    case sf::Key::Num3:
      retString = JS_NewStringCopyZ(cx, "Num3");
      break;
    case sf::Key::Num4:
      retString = JS_NewStringCopyZ(cx, "Num4");
      break;
    case sf::Key::Num5:
      retString = JS_NewStringCopyZ(cx, "Num5");
      break;
    case sf::Key::Num6:
      retString = JS_NewStringCopyZ(cx, "Num6");
      break;
    case sf::Key::Num7:
      retString = JS_NewStringCopyZ(cx, "Num7");
      break;
    case sf::Key::Num8:
      retString = JS_NewStringCopyZ(cx, "Num8");
      break;
    case sf::Key::Num9:
      retString = JS_NewStringCopyZ(cx, "Num9");
      break;
    case sf::Key::Num0:
      retString = JS_NewStringCopyZ(cx, "Num0");
      break;
    case sf::Key::W:
      retString = JS_NewStringCopyZ(cx, "W");
      break;
    case sf::Key::S:
      retString = JS_NewStringCopyZ(cx, "S");
      break;
    case sf::Key::X:
      retString = JS_NewStringCopyZ(cx, "X");
      break;
    case sf::Key::Space:
      retString = JS_NewStringCopyZ(cx, "Space");
      break;
    case sf::Key::Escape:
      retString = JS_NewStringCopyZ(cx, "Escape");
      break;
    default:
      JS_ReportError(cx, "Unable to parse out string value of key code with provided const value of %d", keyCode);
      return JS_FALSE;
  }

  jsval rVal = STRING_TO_JSVAL(retString);
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

static JSFunctionSpec keyObjFunctionSpec[] = {
  JS_FS("toString",keyObj_native_toString,0,0),
  JS_FS_END
};

JSObject* getKeyObjectFor(JSContext* cx, sf::Key::Code keyCode) {
  JSObject* keyObj = JS_NewObject(cx, &keyObjectClassDef, NULL, NULL);
  sf::Key::Code* kcp = new sf::Key::Code;
  *kcp = keyCode;
  if (JS_SetPrivate(cx, keyObj, kcp) != JS_TRUE) {
    JS_ReportError(cx, "getKeyObjectFor: failed to set private keyObj member");
    return NULL;
  }
  if(!JS_DefineFunctions(cx, keyObj, keyObjFunctionSpec))
  {
    JS_ReportError(cx, "getKeyObjectFor: failed to add function spec for keyObj");
    return NULL;
  }
  return keyObj;
}

// Input funcs
JSBool reformer_native_input_isKeyDown(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* This;
  JSObject* kfResult;

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "oo", &This, &kfResult)) {
    JS_ReportError(cx, "reformer_native_input_isKeyDown: unable to parse args");
    return JS_FALSE;
  }

  sf::RenderWindow* window = (sf::RenderWindow*)JS_GetPrivate(cx, This);
  if (window == NULL) {
    JS_ReportError(cx, "reformer_native_input_isKeyDown: unable to pull sf::RenderWindow private member out of This\n");
    return JS_FALSE;
  }
  sf::Key::Code* keyCodeP = (sf::Key::Code*)(JS_GetPrivate(cx, kfResult));
  sf::Key::Code keyCode = *keyCodeP;
  if (keyCode == NULL) {
    JS_ReportError(cx, "reformer_native_input_isKeyDown: unable to pull sf::Key::Code private member out of kfObject\n");
    return JS_FALSE;
  }

  JSBool result = JS_FALSE;
  if(window->GetInput().IsKeyDown(keyCode)) {
    result = JS_TRUE;
  }

  jsval rVal = BOOLEAN_TO_JSVAL(result);
  JS_SET_RVAL(cx, vp, rVal);

  return JS_TRUE;
}

JSBool reformer_native_input_getMousePos(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* This;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "o", &This))
  {
    JS_ReportError(cx, "reformer_native_input_getMousePos: unable to parse args");
    return JS_FALSE;
  }
  sf::RenderWindow* window = (sf::RenderWindow*)JS_GetPrivate(cx, This);
  if(window == NULL)
  {
    JS_ReportError(cx, "reformer_native_input_getMousePos: unable to pull RenderWindow from This's private prop");
    return JS_FALSE;
  }
  JSObject* mPosObj = JS_NewObject(cx, NULL, NULL, NULL);

  jsval xPosVal = INT_TO_JSVAL(window->GetInput().GetMouseX());
  jsval yPosVal = INT_TO_JSVAL(window->GetInput().GetMouseY());

  if(!JS_SetProperty(cx, mPosObj, "x", &xPosVal))
  {
    JS_ReportError(cx, "reformer_native_input_getMousePos: unable to set x prop for return object");
    return JS_FALSE;
  }
  if(!JS_SetProperty(cx, mPosObj, "y", &yPosVal))
  {
    JS_ReportError(cx, "reformer_native_input_getMousePos: unable to set y prop for return object");
    return JS_FALSE;
  }

  jsval rVal = OBJECT_TO_JSVAL(mPosObj);
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

static JSFunctionSpec input_native_functions[] = {
  JS_FS("__native_isKeyDown", reformer_native_input_isKeyDown, 2, 0),
  JS_FS("__native_getMousePos", reformer_native_input_getMousePos, 1, 0),
  JS_FS_END
};

static JSClass keyFinderClassDef = {
  "NativeKeyFinder",
  JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, classdef_keyfinder_resolver, JS_ConvertStub, JS_FinalizeStub
};
JSObject* newKeysFinderObject(JSContext* cx) {
  return JS_NewObject(cx, &keyFinderClassDef, NULL, NULL);
}

static void
classdef_input_finalize(JSContext* cx, JSObject* sp) {
  /*sf::RenderWindow* window = (sf::RenderWindow*)JS_GetPrivate(cx, sp);
  printf("About to try and delete an sf::RenderWindow...\n");
  delete window;*/
}
static JSClass
native_input_classdef = {
  "NativeInput",
  JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, classdef_input_finalize
};

JSObject* sugs::richclient::input::newInputFrom(sf::RenderWindow* window, JSContext* cx) {
  JSObject* input = JS_NewObject(cx, &native_input_classdef, NULL, NULL);
  if (JS_SetPrivate(cx, input, window) != JS_TRUE) {
    JS_ReportError(cx, "Failed to bind RenderWindow to new NativeInput");
    return NULL;
  }
  if (!JS_DefineFunctions(cx, input, input_native_functions)) {
    JS_ReportError(cx, "Failed to define functions for new Input");
    return NULL;
  }

  return input;
}

void sugs::richclient::input::registerInputNatives(JSContext* cx, JSObject* global) {
  JSObject* keyFinder = newKeysFinderObject(cx);
  jsval kfVal = OBJECT_TO_JSVAL(keyFinder);
  JS_SetProperty(cx, global, "Keys", &kfVal);
}

void sugs::richclient::input::pushKeyUpEvent(JSContext* cx, JSObject* global, sf::Key::Code code)
{
  JSObject* keyObj = getKeyObjectFor(cx, code);
  jsval argv[1];
  argv[0] = OBJECT_TO_JSVAL(keyObj);
  jsval rVal;
  JS_CallFunctionName(cx, global, "__pushOnKeyUpEvent", 1, argv, &rVal);
}
