#include <SFML/System.hpp>
#include <iostream>
#include <stdio.h>


#include "../sugs-core/ext/ext.hpp"
#include "../sugs-core/common.hpp"

#include "../sugs-core/core.h"
#include "../sugs-core/messaging/messageexchange.hpp"

#include "speccomponent.hpp"

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
  sugs::core::worker::ConfiguratorWorker* configurator = new sugs::core::worker::ConfiguratorWorker(rt);
  configurator->init();
  sugsConfig lConf = configurator->getConfig();
  *config = configurator->getConfig();
  delete configurator;
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

  sugs::core::worker::Worker* worker = new sugs::core::worker::Worker(msgEx, "spec_runner", config);
  sugs::core::ext::Component* fsComp = new sugs::core::ext::FilesystemComponent();
  sugs::core::ext::Component* specComp = new sugs::spec::SpecComponent(FLAGS_path);
  sugs::core::ext::Component* specRunnerScript = new sugs::core::ext::ScriptRunnerComponent("spec/clirunner.coffee");
  worker->addComponent(fsComp);
  worker->addComponent(specComp);
  worker->addComponent(specRunnerScript);
  worker->start(false);

  // run the $.mainLoop in runner.coffee
  delete worker;      // all of these deletes should go away w/ shared_ptr use
  delete specComp;
  delete msgEx;
  delete specRunnerScript;
  sugs::core::js::shutdownSpidermonkey();

  return 0;
}
