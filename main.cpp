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
  result = executeScript("underscore.js", jsEnv.cx, jsEnv.global);
  if(result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }
  result = executeScript("coffee-script.js", jsEnv.cx, jsEnv.global);
  if(result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }
  result = executeCoffeeScript("kernel.coffee", jsEnv.cx, jsEnv.global);
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
