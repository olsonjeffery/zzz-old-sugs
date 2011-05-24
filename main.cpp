#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <stdio.h>

#include "jssetup.hpp"
#include "graphics.hpp"
#include "medialibrary.hpp"

int main() {
  printf("before js init...\n");
  jsEnv env = initJsEnvironment();

  // load up underscore
  executeScript("underscore.js", env.cx, env.global);
  // load up the main reformer.js script
  executeScript("reformer.js", env.cx, env.global);

  printf("about to init graphics..\n");
  graphicsEnv gfxEnv = initGraphics();

  printf("about to create sprite..\n");
  sf::Sprite playerSprite;
  playerSprite.SetImage(*(MediaLibrary::LoadImage("circle_asterisk.png")));
  playerSprite.SetPosition({ 40, 20});

  sf::Sprite playerSprite2;
  playerSprite2.SetImage(*(MediaLibrary::LoadImage("circle_asterisk.png")));
  playerSprite2.SetPosition({ 250, 100});

  printf("about to open window\n");
  while(gfxEnv.window->IsOpened()) {
    sf::Event Event;

    // event processing
    while(gfxEnv.window->GetEvent(Event)) {
      if (Event.Type == sf::Event::Closed) {
        gfxEnv.window->Close();
      }
    }

    gfxEnv.window->Clear();

    // drawing
    gfxEnv.window->Draw(playerSprite);
    gfxEnv.window->Draw(playerSprite2);

    gfxEnv.window->Display();
  }

  teardownGraphics(gfxEnv.window);

  teardownJsEnvironment(env.rt, env.cx);

  return EXIT_SUCCESS;
}
