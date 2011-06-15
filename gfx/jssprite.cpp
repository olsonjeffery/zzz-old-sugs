#include "jssprite.hpp"

static JSBool
native_sprite_factory(JSContext* cx, uintN argc, jsval* vp) {
  printf("calling native_sprite_factory\n");

  JSString* text;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &text)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "native_sprite_factory: wtf can't parse arguments");
      return JS_FALSE;
  }
  char* textStr = JS_EncodeString(cx, text);
  printf("adding new sprite located, image file: %s\n", textStr);

  sf::Image* img = MediaLibrary::LoadImage(textStr);
  if (img == 0) {
    JS_ReportError(cx, "native_sprite_factory: couldn't load sprite.. rut roh..\n");
    return JS_FALSE;
  }

  sf::Sprite* sfmlSprite = new sf::Sprite();
  sfmlSprite->SetImage(*img);
  double width = img->GetWidth();
  double height = img->GetHeight();
  sfmlSprite->SetOrigin(width / 2.f, height / 2.f);
  sfmlSprite->SetPosition(0,0);

  JSObject* spriteObj = JS_NewObject(cx, getDrawableClassDef(), NULL, NULL);
  JS_DefineFunctions(cx, spriteObj, getDrawableFunctionSpec());
  JS_SetPrivate(cx, spriteObj, sfmlSprite);
  jsval rVal = OBJECT_TO_JSVAL(spriteObj);

  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

void
registerSpriteFactory(JSContext* cx, JSObject* global) {
    JS_DefineFunction(cx, global, "__native_factory_sprite", native_sprite_factory, 1, 0);
}
