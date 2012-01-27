#include "richclientcomponent.hpp"

namespace sugs {
namespace richclient {

void RichClientComponent::componentTeardown(jsEnv jsEnv) {
  printf("frontend dtor...\n");
  jsval argv[0];
  jsval rVal;
  JS_CallFunctionName(jsEnv.cx, jsEnv.global, "showEntryPoints", 0, argv, &rVal);
}

sugs::richclient::gfx::GraphicsEnv createGraphicsEnv(JSContext* cx, int width, int height, int colorDepth) {
  // init graphics
  return sugs::richclient::gfx::initGraphics(cx, width, height, colorDepth);
}

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

  sugs::richclient::gfx::GraphicsEnv gfxEnv = createGraphicsEnv(cx, width, height, colorDepth);
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

void bindGraphicsSetupEnvironments(jsEnv jsEnv, RichClientComponent* rc) {
  // general richclient functions...
  JSObject* richClientObj = JS_NewObject(jsEnv.cx, sugs::common::jsutil::getDefaultClassDef(), NULL, NULL);
  if(!JS_SetPrivate(jsEnv.cx, richClientObj, rc)) {
      JS_ReportError(jsEnv.cx,"RichClientComponent::componentRegisterNativeFunctions: Unable to set privite obj on windowFuncsObj...");
  }
  if(!JS_DefineFunctions(jsEnv.cx, richClientObj, richclientGlobalFuncSpecs)) {
    JS_ReportError(jsEnv.cx,"RichClientComponent/componentRegisterNativeFunctions: Unable to register window funcs obj functions...");
  }

  sugs::common::jsutil::embedObjectInNamespace(jsEnv.cx, jsEnv.global, jsEnv.global, "sugs.api.richclient", richClientObj);
}

void RichClientComponent::registerNativeFunctions(jsEnv jsEnv, pathStrings paths) {
  this->_jsEnv = jsEnv;

  // this needs to go away..
  MediaLibrary::RegisterDefaultFont();

  // some other useful global
  bindGraphicsSetupEnvironments(this->_jsEnv, this);

  // init sfml bindings for the frontend
  sugs::richclient::gfx::registerGraphicsNatives(jsEnv.cx, jsEnv.global);
  sugs::richclient::input::registerInputNatives(jsEnv.cx, jsEnv.global);
}

int msElapsed = 0;
bool RichClientComponent::doWork(jsEnv jsEnv, pathStrings paths) {
  return true;
}


sugs::core::ext::Component* RichClientComponentFactory::create(jsEnv jsEnv, JSObject* configJson)
{
  jsval widthVal;
  if(!JS_GetProperty(jsEnv.cx, configJson, "screenWidth", &widthVal)) {
      printf("RichClientComponentFactory.create(): failure to pull screenWidth from global.configJson\n");
      exit(EXIT_FAILURE);
  }
  int width = SUGS_JSVAL_TO_NUMBER(widthVal);

  jsval heightVal;
  if(!JS_GetProperty(jsEnv.cx, configJson, "screenHeight", &heightVal)) {
      printf("RichClientComponentFactory.create(): failure to pull screenHeight from global.configJson\n");
      exit(EXIT_FAILURE);
  }
  int height = SUGS_JSVAL_TO_NUMBER(heightVal);

  jsval colorDepthVal;
  if(!JS_GetProperty(jsEnv.cx, configJson, "colorDepth", &colorDepthVal)) {
      printf("RichClientComponentFactory.create(): failure to pull colorDepth from global.configJson\n");
      exit(EXIT_FAILURE);
  }
  int colorDepth = SUGS_JSVAL_TO_NUMBER(colorDepthVal);
  return new RichClientComponent(width, height, colorDepth);
}

std::string RichClientComponentFactory::getName()
{
  return "RichClient";
}

}} // namespace sugs::richclient
