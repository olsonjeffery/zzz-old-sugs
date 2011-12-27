#include "gfx.hpp"

namespace sugs {
namespace richclient {
namespace gfx {

static JSBool
native_text_setString(JSContext* cx, uintN argc, jsval*vp) {
  JSObject* This;
  JSString* contentObj;
  if(!JS_ConvertArguments(cx, 2, JS_ARGV(cx,vp), "oS", &This, &contentObj)) {
    JS_ReportError(cx, "native_text_setString: failed to parse args");
    return JS_FALSE;
  }
  char* content = JS_EncodeString(cx, contentObj);

  sf::Text* text = (sf::Text*)JS_GetPrivate(cx, This);
  if (text == NULL) {
    JS_ReportError(cx, "native_text_setString: failed to pull private prop from This");
    return JS_FALSE;
  }

  text->SetString(sf::String(std::string(content)));

  return JS_TRUE;
}

static JSFunctionSpec nativeTextFunctionSpec[] = {
    JS_FS("__native_setString", native_text_setString, 2, 0),
    JS_FS_END
};

static JSBool
native_text_factory(JSContext* cx, uintN argc, jsval* vp) {
  printf("calling native_text_factory\n");

  JSString* contentObj;
  jsdouble fontSize;
  JSString* fontNameObj;
  JSObject* colorParams;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "SdSo", &contentObj, &fontSize, &fontNameObj, &colorParams)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "native_text_factory: can't parse arguments");
      return JS_FALSE;
  }
  char* content = JS_EncodeString(cx, contentObj);
  char* fontName = JS_EncodeString(cx, fontNameObj);

  sf::Color fontColor;
  if (!getColorFrom(cx, colorParams, &fontColor)) {
    JS_ReportError(cx, "native_text_factory: failed to parse out fontColor");
    return JS_FALSE;
  }

  printf("content of fontName input: '%s'\n", fontName);
  sf::Font* font = MediaLibrary::LoadFont(std::string(fontName));
  if (font == 0) {
    JS_ReportError(cx, "native_text_factory: couldn't find a valid font with provided input of '%s'", fontName);
    return JS_FALSE;
  }
  sf::String contentStr = sf::String(content);
  sf::Text* sfmlText = new sf::Text(contentStr, *font, fontSize);
  sfmlText->SetPosition(0,0);
  sfmlText->SetColor(fontColor);

  JSObject* rectObj = JS_NewObject(cx, getDrawableClassDef(), NULL, NULL);
  JS_DefineFunctions(cx, rectObj, getDrawableFunctionSpec());
  JS_DefineFunctions(cx, rectObj, nativeTextFunctionSpec);
  JS_SetPrivate(cx, rectObj, sfmlText);
  jsval rVal = OBJECT_TO_JSVAL(rectObj);

  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

void
registerTextFactory(JSContext* cx, JSObject* global) {
    JS_DefineFunction(cx, global, "__native_factory_text", native_text_factory, 4, 0);
}

}}} // namespace sugs::richclient::gfx
