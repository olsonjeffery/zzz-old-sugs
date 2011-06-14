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

  // The introduction of user code
  if (fileExists("config.js")) {
    result = executeScript("config.js", jsEnv.cx, jsEnv.global);
    if(result.result == JS_FALSE) {
      printf(result.message);
      exit(EXIT_FAILURE);
    }
  }
  else {
    result = executeCoffeeScript("config.coffee", jsEnv.cx, jsEnv.global);
    if(result.result == JS_FALSE) {
      printf(result.message);
      exit(EXIT_FAILURE);
    }
  }

  // init graphics
  graphicsEnv gfxEnv = initGraphics(jsEnv.cx);
  eventEnv evEnv = {
    newInputFrom(gfxEnv.window, jsEnv.cx)
  };

  // run $.startup() in user code
  result = execStartupCallbacks(jsEnv);
  if (result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }

  /*****************
    MAIN GAME LOOP
  *****************/
  printf("beginning main game loop\n");

  float frametime;
  float framerate;
  while(gfxEnv.window->IsOpened()) {
    sf::Event Event;
    frametime = gfxEnv.window->GetFrameTime() * 0.001f;
    framerate = 1.f / frametime;

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
    callIntoJsRender(jsEnv, gfxEnv, evEnv, frametime, framerate);

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
