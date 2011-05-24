#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <stdio.h>

#include "jsmapping.hpp"
#include "graphics.hpp"

int main() {
  printf("before js init...\n");
  jsEnv env = initJsEnvironment();

  // load up underscore
  executeScript("underscore.js", env.cx, env.global);
  // load up the main reformer.js script
  executeScript("reformer.js", env.cx, env.global);

  graphicsEnv gfx = initGraphics();

  while(gfx.window->IsOpened()) {
    sf::Event Event;
    while(gfx.window->GetEvent(Event)) {
      if (Event.Type == sf::Event::Closed) {
        gfx.window->Close();
      }
    }

    gfx.window->Clear();

    gfx.window->Display();
  }

  teardownJsEnvironment(env.rt, env.cx);

  return EXIT_SUCCESS;
}
