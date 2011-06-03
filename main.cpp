#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <stdio.h>

#include "jssetup.hpp"
#include "graphics.hpp"
#include "jsgraphics.hpp"
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
  result = executeCoffeeScript("core.coffee", jsEnv.cx, jsEnv.global);
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

  result = execStartupCallbacks(jsEnv);
  if (result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }

  /*****************
    MAIN GAME LOOP
  *****************/
  printf("about to open window\n");
  while(gfxEnv.window->IsOpened()) {
    sf::Event Event;

    // event processing
    while(gfxEnv.window->GetEvent(Event)) {
      if (Event.Type == sf::Event::Closed) {
        gfxEnv.window->Close();
      }
    }

    // BEGINNING OF THE DRAW/RENDER LOOP
    gfxEnv.window->Clear();

    // drawing
    callIntoJsRender(jsEnv, gfxEnv, evEnv);

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
