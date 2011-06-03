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

JSObject* getKeyObjectFor(JSContext* cx, sf::Key::Code keyCode) {
  JSObject* keyObj = JS_NewObject(cx, &keyObjectClassDef, NULL, NULL);
  sf::Key::Code* kcp = new sf::Key::Code;
  *kcp = keyCode;
  if (JS_SetPrivate(cx, keyObj, kcp) != JS_TRUE) {
    JS_ReportError(cx, "getKeyObjectFor: failed to set private keyObj member");
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

static JSFunctionSpec input_native_functions[] = {
  JS_FS("__native_isKeyDown", reformer_native_input_isKeyDown, 2, 0),
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

JSObject* newInputFrom(sf::RenderWindow* window, JSContext* cx) {
  JSObject* input = JS_NewObject(cx, NULL, NULL, NULL);
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

void registerInputNatives(JSContext* cx, JSObject* global) {
  JSObject* keyFinder = newKeysFinderObject(cx);
  jsval kfVal = OBJECT_TO_JSVAL(keyFinder);
  JS_SetProperty(cx, global, "Keys", &kfVal);
}
