#include "jsgraphics.hpp"

JSBool reformer_native_sprite_setPosition(JSContext* cx, uintN argc, jsval* vp)
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

  sf::Sprite* sprite = (sf::Sprite*)(JS_GetPrivate(cx, This));
  sprite->SetPosition(x, y);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}
JSBool reformer_native_sprite_getPosition(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* This;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "o", &This)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "wtf can't parse arguments for getting pos obj", 1);
      return JS_FALSE;
  }
  sf::Sprite* sprite = (sf::Sprite*)(JS_GetPrivate(cx, This));
  sf::Vector2f pos = sprite->GetPosition();
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
JSBool reformer_native_sprite_move(JSContext* cx, uintN argc, jsval* vp)
{
  // note that, here, we're basically saying that this native function
  // is only meant to be used as a method, because we have a This.. currently
  // requires some legwork to get the enclosing object.. until I can find a better
  // way..
  JSObject* This;
  JSObject* posObj;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "oo", &This, &posObj)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "reformer_native_sprite_move: can't parse out arguments", 1);
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

  sf::Sprite* sprite = (sf::Sprite*)(JS_GetPrivate(cx, This));
  printf("moving sprite --  x: %f y: %f\n", x, y);
  sprite->Move(x, y);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}
static JSFunctionSpec sprite_native_functions[] = {
  JS_FS("__native_setPos", reformer_native_sprite_setPosition, 2, 0),
  JS_FS("__native_getPos", reformer_native_sprite_getPosition, 1, 0),
  JS_FS("__native_move", reformer_native_sprite_move, 2, 0),
  JS_FS_END
};

static void
classdef_sprite_finalize(JSContext* cx, JSObject* sp) {
  sf::Sprite* sprite = (sf::Sprite*)JS_GetPrivate(cx, sp);
  delete sprite;
}
static JSClass spriteClassDef = {
  "NativeSprite",
  JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, classdef_sprite_finalize
};

/* NATIVE FUNCTIONS */
JSBool reformer_native_newSprite(JSContext* cx, uintN argc, jsval* vp)
{
  printf("calling __native_newSprite\n");

  JSString* text;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &text)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "wtf can't parse arguments", 1);
      return JS_FALSE;
  }
  char* textStr = JS_EncodeString(cx, text);
  printf("adding new sprite located, image file: %s\n", textStr);

  sf::Image* img = MediaLibrary::LoadImage(textStr);
  if (img == 0) {
    printf("couldn't load sprite.. rut roh..\n");
    return JS_FALSE;
  }
  sf::Sprite* sfmlSprite = new sf::Sprite();
  sfmlSprite->SetImage(*img);

  JSObject* spriteObj = JS_NewObject(cx, &spriteClassDef, NULL, NULL);
  JS_DefineFunctions(cx, spriteObj, sprite_native_functions);
  JS_SetPrivate(cx, spriteObj, sfmlSprite);
  jsval rVal = OBJECT_TO_JSVAL(spriteObj);

  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

static JSFunctionSpec graphics_native_functions[] = {
  JS_FS("__native_newSprite", reformer_native_newSprite, 1, 0),
  JS_FS_END
};

/*
 *
 * PUBLIC FUNCTIONS
 *
 */
void callIntoJsRender(jsEnv jsEnv, graphicsEnv gfxEnv, eventEnv evEnv, double frametime, double framerate) {
  jsval argv[4];

  argv[0] = OBJECT_TO_JSVAL(gfxEnv.canvas);
  argv[1] = OBJECT_TO_JSVAL(evEnv.input);
  argv[2] = DOUBLE_TO_JSVAL(frametime);
  argv[3] = DOUBLE_TO_JSVAL(framerate);
  jsval rval;
  JS_CallFunctionName(jsEnv.cx, jsEnv.global, "renderSprites", 4, argv, &rval);
}

void registerGraphicsNatives(JSContext* cx, JSObject* global) {
  if (!JS_DefineFunctions(cx, global, graphics_native_functions)) {
    printf("failure to declare graphics functions");
    exit(EXIT_FAILURE);
  }
}

