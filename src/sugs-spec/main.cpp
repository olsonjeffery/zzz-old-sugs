#include <SFML/System.hpp>
#include <iostream>
#include <stdio.h>

#include "../sugs-core/ext/component.h"
#include "../sugs-core/common.hpp"

#include "../sugs-core/worker/spidermonkey/jssetup.hpp"
#include "../sugs-core/worker/configurator.hpp"
#include "../sugs-core/worker/backend.hpp"
#include "../sugs-core/messaging/messageexchange.hpp"

void getConfig(JSRuntime* rt, sugsConfig* config) {
  jsEnv jsEnv = initContext(rt);
  ConfiguratorWorker configurator(rt);
  configurator.initLibraries();
  sugsConfig lConf = configurator.getConfig();
  *config = configurator.getConfig();
}

const int vmMemSize = (((1024L) * 1024L) * 1024L);

int main()
{
  predicateResult result;

  Rng::initialize();
  // initialize the in-process messenger facility
  MessageExchange* msgEx = new MessageExchange();

  printf("setting up runtime and fetching config...\n");
  JSRuntime* rt = initRuntime(vmMemSize);
  sugsConfig config;
  getConfig(rt, &config);

  std::cout << "sugs-spec" << std::endl;

  return 0;
}
