#include "richclientcomponent.hpp"

namespace sugs {
namespace richclient {

static JSBool
native_create2DAcceleratedCanvas(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* optionsObj;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "o", &optionsObj)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "native_create2DAcceleratedCanvas: can't parse arguments");
      return JS_FALSE;
  }

  jsval widthVal;
  if(!JS_GetProperty(cx, optionsObj, "width", &widthVal)) {
    JS_ReportError(cx, "native_rectangle_factory: failed to pull width val from rectParams");
    return JS_FALSE;
  }
  double width = SUGS_JSVAL_TO_NUMBER(widthVal);

  jsval heightVal;
  if(!JS_GetProperty(cx, optionsObj, "height", &heightVal)) {
    JS_ReportError(cx, "native_rectangle_factory: failed to pull height val from rectParams");
    return JS_FALSE;
  }
  double height = SUGS_JSVAL_TO_NUMBER(heightVal);

  jsval colorDepthVal;
  if(!JS_GetProperty(cx, optionsObj, "colorDepth", &colorDepthVal)) {
    JS_ReportError(cx, "native_rectangle_factory: failed to pull height val from rectParams");
    return JS_FALSE;
  }
  double colorDepth = SUGS_JSVAL_TO_NUMBER(colorDepthVal);

  sugs::richclient::gfx::GraphicsEnv gfxEnv = sugs::richclient::gfx::initGraphics(cx, width, height, colorDepth);
  JSObject* canvas = gfxEnv.canvas;

  jsval rVal = OBJECT_TO_JSVAL(canvas);
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

static JSFunctionSpec
richclientGlobalFuncSpecs[] = {
  JS_FS("create2DAcceleratedCanvas", native_create2DAcceleratedCanvas, 3, 0),
  JS_FS_END
};

void RichClientComponent::setup(jsEnv jsEnv, pathStrings paths) {
  // this needs to go away..
  MediaLibrary::RegisterDefaultFont();

  // general richclient functions...
  JSObject* richClientObj = JS_NewObject(jsEnv.cx, sugs::common::jsutil::getDefaultClassDef(), NULL, NULL);
  if(!JS_DefineFunctions(jsEnv.cx, richClientObj, richclientGlobalFuncSpecs)) {
    JS_ReportError(jsEnv.cx,"RichClientComponent/componentRegisterNativeFunctions: Unable to register window funcs obj functions...");
  }
  // probably should combine all of these into one global namespace object
  sugs::common::jsutil::embedObjectInNamespace(jsEnv.cx, jsEnv.global, "sugs.api.richclient", richClientObj);

  sugs::richclient::gfx::registerGraphicsNatives(jsEnv.cx, jsEnv.global);
  sugs::richclient::input::registerInputNatives(jsEnv.cx, jsEnv.global);
}

sugs::core::ext::Component* RichClientComponentFactory::create(jsEnv jsEnv, JSObject* configJson)
{
  return new RichClientComponent();
}

std::string RichClientComponentFactory::getName()
{
  return "RichClient";
}

}} // namespace sugs::richclient
