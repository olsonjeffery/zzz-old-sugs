#include "jsinput.hpp"

JSObject* getKeyObjectFor(JSContext* cx, sf::Key::Code keyCode);
JSObject* newKeysFinderObject(JSContext* cx);

static JSBool
reformer_native_input_resolver(JSContext *cx, JSObject *obj, jsid id) {
  jsval idVal;
  if (JS_IdToValue(cx, id, &idVal) != JS_TRUE) {
    JS_ReportError(cx, "Unable to convert jsid to jsval");
    return JS_FALSE;
  }

  JSString* propNameJsString = JSVAL_TO_STRING(idVal);
  char* propName = JS_EncodeString(cx, propNameJsString);

  JSObject* keyObj = NULL;
  if (strcmp(propName, "Up")) {
    keyObj = getKeyObjectFor(cx, sf::Key::Up);
  }
  else if (strcmp(propName, "Down")) {
    keyObj = getKeyObjectFor(cx, sf::Key::Down);
  }
  else if (strcmp(propName, "Left")) {
    keyObj = getKeyObjectFor(cx, sf::Key::Left);
  }
  else if (strcmp(propName, "Right")) {
    keyObj = getKeyObjectFor(cx, sf::Key::Right);
  }
  else {
    JS_ReportError(cx, "Unable to find matching key for %s", propName);
    keyObj = NULL;
    printf("did not find matching key");
    return JS_FALSE;
  }

  if (keyObj != NULL) {
    printf("found matching key?\n");
    jsval propVal = OBJECT_TO_JSVAL(keyObj);
    JS_SetProperty(cx, obj, propName, &propVal);
  }


  return JS_TRUE;
}

JSObject* getKeyObjectFor(JSContext* cx, sf::Key::Code keyCode) {
  JSObject* keyObj = JS_NewObject(cx, NULL, NULL, NULL);
  sf::Key::Code* kcp = &keyCode;
  if (JS_SetPrivate(cx, keyObj, kcp) != JS_TRUE) {
    JS_ReportError(cx, "getKeyObjectFor: failed to set private keyObj member");
    return NULL;
  }
  return keyObj;
}

void keypress() {

}

static JSClass keyFinderClassDef = {
  "NativeKeyFinder",
  JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, reformer_native_input_resolver, JS_ConvertStub, JS_FinalizeStub
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

  return input;
}

void registerInputNatives(JSContext* cx, JSObject* global) {
  JSObject* keyFinder = newKeysFinderObject(cx);
  jsval kfVal = OBJECT_TO_JSVAL(keyFinder);
  JS_SetProperty(cx, global, "Keys", &kfVal);
}
