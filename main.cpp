#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <stdio.h>

#include "jsmapping.hpp"

int main() {
  printf("before js init...\n");
  jsEnv env = initJsEnvironment();

  executeScript("reformer.js", env.cx, env.global);

  teardownJsEnvironment(env.rt, env.cx);

  return EXIT_SUCCESS;
}
