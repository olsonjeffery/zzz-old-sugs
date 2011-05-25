#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <stdio.h>

#include "jssetup.hpp"
#include "graphics.hpp"
#include "jsgraphics.hpp"
#include "medialibrary.hpp"

int main() {
  jsEnv jsEnv = initJsEnvironment();
  graphicsEnv gfxEnv = initGraphics();

  printf("loading javascript libraries...\n");
  // load up underscore
  executeScript("underscore.js", jsEnv.cx, jsEnv.global);
  // load up the main reformer.js library script
  executeScript("reformer.js", jsEnv.cx, jsEnv.global);
  // the actual game script
  printf("loading game script...\n");
  executeScript("game.js", jsEnv.cx, jsEnv.global);

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

    // BEGINNING OF THE DRAW/RENDER LOOP
    gfxEnv.window->Clear();

    callIntoJsRender(jsEnv, gfxEnv);
    // drawing
    gfxEnv.window->Draw(playerSprite);
    gfxEnv.window->Draw(playerSprite2);

    gfxEnv.window->Display();
    // END OF DRAW/RENDER LOOP
  }

  teardownGraphics(gfxEnv.window);

  teardownJsEnvironment(jsEnv.rt, jsEnv.cx);

  return EXIT_SUCCESS;
}
