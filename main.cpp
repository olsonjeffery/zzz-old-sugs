#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <stdio.h>

#include "jsmapping.hpp"

int main() {
  printf("before js init...\n");
  jsEnv env = initJsEnvironment();

  sf::Clock Clock;
  while (Clock.GetElapsedTime() < 5.f) {
    std::cout << Clock.GetElapsedTime() << std::endl;
    sf::Sleep(0.5f);
  }

  teardownJsEnvironment(env.rt, env.cx);

  return EXIT_SUCCESS;
}
