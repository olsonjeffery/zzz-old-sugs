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

void FrontendWorker::initLibraries() {
  //load core libs
  printf("inside frontend loadSugsLibraries()...\n");
  this->loadConfig(this->_config);
  this->loadSugsLibraries(this->_config.paths);

  // init sfml bindings for the frontend
  registerGraphicsNatives(this->_jsEnv.cx, this->_jsEnv.global);
  registerInputNatives(this->_jsEnv.cx, this->_jsEnv.global);

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
void FrontendWorker::doWork() {
  this->processPendingMessages();

  sf::Event Event;

  msElapsed = this->_gfxEnv.window->GetFrameTime();
  // check for window close/quit event..
  while(this->_gfxEnv.window->PollEvent(Event)) {
    if (Event.Type == sf::Event::Closed) {
      this->_gfxEnv.window->Close();
      this->_isClosed = JS_TRUE;
    }
  }

  // BEGINNING OF THE DRAW/RENDER LOOP
  this->_gfxEnv.window->Clear();

  // run $.mainLoop() and $.render() callbacks in
  // user code
  callIntoJsRender(this->_jsEnv, this->_gfxEnv, this->_evEnv, msElapsed);

  this->_gfxEnv.window->Display();
  // END OF DRAW/RENDER LOOP
}

bool FrontendWorker::isWindowClosed() {
  return this->_isClosed;
}
