#include <SFML/System.hpp>
#include <iostream>
#include <stdio.h>


#include "../sugs-core/ext/component.h"
#include "../sugs-core/common.hpp"

#include "../sugs-core/core.h"
#include "../sugs-core/worker/configurator.hpp"
#include "../sugs-core/worker/backend.hpp"
#include "../sugs-core/messaging/messageexchange.hpp"

#include "speccomponent.h"

// gflags declaration
#include <gflags/gflags.h>
static bool ValidatePath(const char* flagname, const std::string& value) {
  return true;
}
DEFINE_string(path, "./",
  "Semi-colon separated list of paths to recursively search for spec scripts to run. Spec scripts are filenames that begin or end with 'test', 'tests', 'spec' or 'specs' (case-insensitive).");
static const bool path_dummy = google::RegisterFlagValidator(&FLAGS_path, &ValidatePath);

void getConfig(JSRuntime* rt, sugsConfig* config) {
  jsEnv jsEnv = sugs::core::js::initContext(rt);
  ConfiguratorWorker configurator(rt);
  configurator.initLibraries();
  sugsConfig lConf = configurator.getConfig();
  *config = configurator.getConfig();
}

const int vmMemSize = (((1024L) * 1024L) * 1024L);

int main(int argc, char *argv[])
{
  google::ParseCommandLineFlags(&argc, &argv, true);

  predicateResult result;

  Rng::initialize();
  // initialize the in-process messenger facility
  MessageExchange* msgEx = new MessageExchange();

  printf("setting up runtime and fetching config...\n");
  JSRuntime* rt = sugs::core::js::initRuntime(vmMemSize);
  sugsConfig config;
  getConfig(rt, &config);

  std::cout << "sugs-spec" << std::endl;
  std::cout << "path: '"<< FLAGS_path << "'" << std::endl;

  sugs::ext::Component* fsComp = new sugs::core::fs::FilesystemComponent();
  sugs::ext::Component* specComp = new sugs::spec::SpecComponent(FLAGS_path);

  BackendWorker* worker = new BackendWorker(rt, config, "spec/clirunner.coffee", msgEx);

  worker->addComponent(fsComp);
  worker->addComponent(specComp);
  worker->initLibraries();

  // run the $.mainLoop in runner.coffee
  worker->doWork();
  delete worker;      // all of these deletes should go away w/ shared_ptr use
  delete specComp;
  delete msgEx;
  sugs::core::js::teardownRuntime(rt);
  sugs::core::js::shutdownSpidermonkey();

  return 0;
}
