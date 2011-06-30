#include "configurator.hpp"

void ConfiguratorWorker::parseConfigFile() {
  this->_config = execConfig(this->_jsEnv.cx, this->_jsEnv.global);
}

void ConfiguratorWorker::initLibraries() {
  this->loadConfig(this->_config);
  this->loadSugsLibraries(this->_config.paths);
}

sugsConfig ConfiguratorWorker::getConfig() {
  return this->_config;
}

workerInfos ConfiguratorWorker::getWorkerInfos() {
  this->_workers = getWorkerInfo(this->_jsEnv.cx, this->_jsEnv.global, this->_config);
  return this->_workers;
}
