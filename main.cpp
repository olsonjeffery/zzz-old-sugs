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

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <stdio.h>

#include "jssetup.hpp"
#include "gfx/sfmlsetup.hpp"
#include "gfx/jsgraphics.hpp"
#include "jsinput.hpp"
#include "medialibrary.hpp"

int main() {

  /**********************
    ALL THE SETUP STUFF
  **********************/
  predicateResult result;
  jsEnv jsEnv = initJsEnvironment();

  // Load up our core dependencies
  result = executeScript("extlibs/js/underscore.js", jsEnv.cx, jsEnv.global);
  if(result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }
  result = executeScript("extlibs/js/coffee-script.js", jsEnv.cx, jsEnv.global);
  if(result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }
  result = executeCoffeeScript("sugs.coffee", jsEnv.cx, jsEnv.global);
  if(result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }

  sugsConfig config = execConfig(jsEnv);

  // init graphics
  graphicsEnv gfxEnv = initGraphics(jsEnv.cx, config);
  eventEnv evEnv = {
    newInputFrom(gfxEnv.window, jsEnv.cx)
  };

  printf("gonna execute %s %d\n", config.moduleEntryPoint, config.entryPointIsCoffee);

  if (config.entryPointIsCoffee == JS_TRUE) {
    printf("entry point is .coffee\n");
    executeCoffeeScript(config.moduleEntryPoint, jsEnv.cx, jsEnv.global);
  }
  else {
    printf("entry point is .js\n");
    executeScript(config.moduleEntryPoint, jsEnv.cx, jsEnv.global);
  }

    // some misc pre-startup things
  MediaLibrary::RegisterDefaultFont();

  // run $.startup() in user code
  result = execStartupCallbacks(jsEnv);
  if (result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }

  /*****************
    MAIN GAME LOOP
  *****************/
  printf("entering main game loop\n");

  int msElapsed = 0;
  while(gfxEnv.window->IsOpened()) {
    sf::Event Event;

    msElapsed = gfxEnv.window->GetFrameTime();
    // check for window close/quit event..
    while(gfxEnv.window->PollEvent(Event)) {
      if (Event.Type == sf::Event::Closed) {
        gfxEnv.window->Close();
      }
    }

    // BEGINNING OF THE DRAW/RENDER LOOP
    gfxEnv.window->Clear();

    // run $.mainLoop() and $.render() callbacks in
    // user code
    callIntoJsRender(jsEnv, gfxEnv, evEnv, msElapsed);

    gfxEnv.window->Display();
    // END OF DRAW/RENDER LOOP
  }

  /*************
    TEAR DOWN
  *************/
  teardownGraphics(gfxEnv.window, gfxEnv.canvas, jsEnv.cx);

  teardownJsEnvironment(jsEnv.rt, jsEnv.cx);

  return EXIT_SUCCESS;
}
