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

#include "frontend.hpp"

void FrontendWorker::componentSetup(jsEnv jsEnv, sugsConfig config) {
  printf("initializing graphics.. sw: %d\n", config.screenWidth);
  // init graphics
  this->_gfxEnv = initGraphics(jsEnv.cx, config);
  this->_evEnv = {
    sugs::richclient::input::newInputFrom(this->_gfxEnv.window, jsEnv.cx)
  };

  // set up graphics libs
  MediaLibrary::RegisterDefaultFont();
}

void FrontendWorker::componentTeardown(jsEnv jsEnv) {
  printf("frontend dtor...\n");
  jsval argv[0];
  jsval rVal;
  JS_CallFunctionName(jsEnv.cx, jsEnv.global, "showEntryPoints", 0, argv, &rVal);
  teardownGraphics(this->_gfxEnv.window, this->_gfxEnv.canvas, jsEnv.cx);
}

void FrontendWorker::componentRegisterNativeFunctions(jsEnv jsEnv, sugsConfig config) {
  // init sfml bindings for the frontend
  registerGraphicsNatives(jsEnv.cx, jsEnv.global);
  sugs::richclient::input::registerInputNatives(jsEnv.cx, jsEnv.global);
}

void FrontendWorker::initLibraries() {
  //load core libs
  printf("inside frontend loadSugsLibraries()...\n");
  this->loadConfig(this->_config);
  this->loadSugsLibraries(this->_config.paths);

  //this->loadComponents(this->_config);
  this->componentRegisterNativeFunctions(this->_jsEnv, this->_config);

  printf("load frontend entry point: %s\n", this->_entryPoint.c_str());
  this->loadEntryPointScript(this->_entryPoint.c_str(), this->_config.paths);
  printf("after frontend entry point...\n");

  predicateResult result;
  // run $.startup() in user code
  result = execStartupCallbacks(this->_jsEnv);
  if (result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }
}

void callIntoJsRender(jsEnv jsEnv, graphicsEnv gfxEnv, eventEnv evEnv, int msElapsed) {
  jsval argv[3];

  argv[0] = OBJECT_TO_JSVAL(gfxEnv.canvas);
  argv[1] = OBJECT_TO_JSVAL(evEnv.input);
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

  // run $.mainLoop() and $.render() callbacks in
  // user code
  callIntoJsRender(jsEnv, this->_gfxEnv, this->_evEnv, msElapsed);

  this->_gfxEnv.window->Display();
  // END OF DRAW/RENDER LOOP
}

void FrontendWorker::doWork() {
  this->processPendingMessages();

  this->componentDoWork(this->_jsEnv);
}

bool FrontendWorker::isWindowClosed() {
  return this->_isClosed;
}
