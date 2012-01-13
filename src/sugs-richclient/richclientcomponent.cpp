#include "richclientcomponent.hpp"

namespace sugs {
namespace richclient {

void RichClientComponent::componentTeardown(jsEnv jsEnv) {
  printf("frontend dtor...\n");
  jsval argv[0];
  jsval rVal;
  JS_CallFunctionName(jsEnv.cx, jsEnv.global, "showEntryPoints", 0, argv, &rVal);
  sugs::richclient::gfx::teardownGraphics(this->_gfxEnv.window, this->_gfxEnv.canvas, jsEnv.cx);
}

static JSBool
native_window_exit(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* thisObj = JS_THIS_OBJECT(cx, vp);
  RichClientComponent* rcc = (RichClientComponent*)JS_GetPrivate(cx, thisObj);
  rcc->closeApp();
  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSFunctionSpec
windowFuncSpecs[] = {
  JS_FS("exit", native_window_exit, 0, 0),
  JS_FS_END
};

sugs::richclient::gfx::GraphicsEnv graphicsSetup(jsEnv jsEnv, sugsConfig config, int width, int height, int colorDepth) {
  // set up media repositories
  MediaLibrary::RegisterDefaultFont();

  // init graphics
  return sugs::richclient::gfx::initGraphics(jsEnv.cx, config, width, height, colorDepth);
}

void RichClientComponent::registerNativeFunctions(jsEnv jsEnv, sugsConfig config) {
  this->_jsEnv = jsEnv;
  this->_gfxEnv = graphicsSetup(this->_jsEnv, config, this->_screenWidth, this->_screenHeight, this->_colorDepth);

  predicateResult result = sugs::core::js::findAndExecuteScript("richclient.coffee", config.paths, jsEnv.cx, jsEnv.global);
  if(result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }

  // init sfml bindings for the frontend
  sugs::richclient::gfx::registerGraphicsNatives(jsEnv.cx, jsEnv.global);
  sugs::richclient::input::registerInputNatives(jsEnv.cx, jsEnv.global);

  // general richclient functions...
  JSObject* windowFuncsObj = JS_NewObject(jsEnv.cx, sugs::common::jsutil::getDefaultClassDef(), NULL, NULL);
  if(!JS_SetPrivate(jsEnv.cx, windowFuncsObj, this)) {
      JS_ReportError(jsEnv.cx,"RichClientComponent::componentRegisterNativeFunctions: Unable to set privite obj on windowFuncsObj...");
  }
  if(!JS_DefineFunctions(jsEnv.cx, windowFuncsObj, windowFuncSpecs)) {
    JS_ReportError(jsEnv.cx,"RichClientComponent/componentRegisterNativeFunctions: Unable to register window funcs obj functions...");
  }

  sugs::common::jsutil::embedObjectInNamespace(jsEnv.cx, jsEnv.global, jsEnv.global, "sugs.richclient.window", windowFuncsObj);
}

void callIntoJsRichClientRender(jsEnv jsEnv, JSObject* canvas, JSObject* input, int msElapsed) {
  jsval argv[3];

  argv[0] = OBJECT_TO_JSVAL(canvas);
  argv[1] = OBJECT_TO_JSVAL(input);
  argv[2] = INT_TO_JSVAL(msElapsed);
  jsval rval;
  JS_CallFunctionName(jsEnv.cx, jsEnv.global, "runRender", 3, argv, &rval);
}

int msElapsed = 0;
bool RichClientComponent::doWork(jsEnv jsEnv, sugsConfig config) {
  sf::Event ev;

  msElapsed = this->_gfxEnv.window->GetFrameTime();
  // check for window close/quit event..
  while(this->_gfxEnv.window->PollEvent(ev)) {
    if (ev.Type == sf::Event::Closed) {
      this->_gfxEnv.window->Close();
      this->_isClosed = JS_TRUE;
    }
    if (ev.Type == sf::Event::KeyReleased) {
      sf::Key::Code code = ev.Key.Code;
      sugs::richclient::input::pushKeyUpEvent(jsEnv.cx, jsEnv.global, code);
    }
  }

  // BEGINNING OF THE DRAW/RENDER LOOP
  this->_gfxEnv.window->Clear();

  JSObject* inputObj = sugs::richclient::input::newInputFrom(this->_gfxEnv.window, jsEnv.cx);
  JSObject* canvas = sugs::richclient::gfx::newCanvasFrom(this->_gfxEnv.window, jsEnv.cx);
  // run $.mainLoop() and $.render() callbacks in
  // user code
  callIntoJsRichClientRender(jsEnv, canvas, inputObj, msElapsed);

  this->_gfxEnv.window->Display();
  // END OF DRAW/RENDER LOOP

  return !this->_isClosed;
}

void RichClientComponent::closeApp()
{
  this->_isClosed = true;
}

bool RichClientComponent::isWindowClosed() {
  return this->_isClosed;
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
