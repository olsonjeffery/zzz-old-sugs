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

#include "frontendworker.h"

namespace sugs {
namespace richclient {

void FrontendWorker::componentSetup(jsEnv jsEnv, sugsConfig config) {
  printf("initializing graphics.. sw: %d\n", config.screenWidth);
  // init graphics
  this->_gfxEnv = sugs::richclient::gfx::initGraphics(jsEnv.cx, config);
  // set up graphics libs
  MediaLibrary::RegisterDefaultFont();
}

void FrontendWorker::componentTeardown(jsEnv jsEnv) {
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
  FrontendWorker* frontend = (FrontendWorker*)JS_GetPrivate(cx, thisObj);
  frontend->closeApp();
  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSFunctionSpec
windowFuncSpecs[] = {
  JS_FS("exit", native_window_exit, 0, 0),
  JS_FS_END
};

void FrontendWorker::componentRegisterNativeFunctions(jsEnv jsEnv, sugsConfig config) {
  // init sfml bindings for the frontend
  sugs::richclient::gfx::registerGraphicsNatives(jsEnv.cx, jsEnv.global);
  sugs::richclient::input::registerInputNatives(jsEnv.cx, jsEnv.global);

  // general richclient functions...
  JSObject* windowFuncsObj = JS_NewObject(jsEnv.cx, sugs::common::jsutil::getDefaultClassDef(), NULL, NULL);
  if(!JS_SetPrivate(jsEnv.cx, windowFuncsObj, this)) {
      JS_ReportError(jsEnv.cx,"FrontendWorker::componentRegisterNativeFunctions: Unable to set privite obj on windowFuncsObj...");
  }
  if(!JS_DefineFunctions(jsEnv.cx, windowFuncsObj, windowFuncSpecs)) {
    JS_ReportError(jsEnv.cx,"FrontendWorker::componentRegisterNativeFunctions: Unable to register window funcs obj functions...");
  }

  sugs::common::jsutil::embedObjectInNamespace(jsEnv.cx, jsEnv.global, jsEnv.global, "sugs.richclient.window", windowFuncsObj);
}

void FrontendWorker::initLibraries() {
  //load core libs
  printf("inside frontend loadSugsLibraries()...\n");
  this->loadConfig(this->_config);
  this->loadSugsLibraries(this->_config.paths);

  predicateResult result = sugs::core::js::findAndExecuteScript("richclient.coffee", this->_config.paths, this->_jsEnv.cx, this->_jsEnv.global);
  if(result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }

  //this->loadComponents(this->_config);
  this->componentRegisterNativeFunctions(this->_jsEnv, this->_config);

  printf("load frontend entry point: %s\n", this->_entryPoint.c_str());
  this->loadEntryPointScript(this->_entryPoint.c_str(), this->_config.paths);
  printf("after frontend entry point...\n");

  // run $.startup() in user code
  result = sugs::core::js::execStartupCallbacks(this->_jsEnv);
  if (result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }
}

void callIntoJsRender(jsEnv jsEnv, JSObject* canvas, JSObject* input, int msElapsed) {
  jsval argv[3];

  argv[0] = OBJECT_TO_JSVAL(canvas);
  argv[1] = OBJECT_TO_JSVAL(input);
  argv[2] = INT_TO_JSVAL(msElapsed);
  jsval rval;
  JS_CallFunctionName(jsEnv.cx, jsEnv.global, "runRender", 3, argv, &rval);
}

int msElapsed = 0;
void FrontendWorker::componentDoWork(jsEnv jsEnv) {
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
  callIntoJsRender(jsEnv, canvas, inputObj, msElapsed);

  this->_gfxEnv.window->Display();
  // END OF DRAW/RENDER LOOP
}

void FrontendWorker::doWork() {
  this->processPendingMessages();

  this->componentDoWork(this->_jsEnv);
}

void FrontendWorker::closeApp()
{
  this->_isClosed = true;
}

bool FrontendWorker::isWindowClosed() {
  return this->_isClosed;
}

}} // namespace sugs::richclient
