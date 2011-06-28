#include "configurator.hpp"

void ConfiguratorWorker::initLibraries() {
  this->loadSugsLibraries();
}

sugsConfig ConfiguratorWorker::getConfig() {
  this->_config = execConfig(this->_jsEnv.cx, this->_jsEnv.global);
  return this->_config;
}

workerInfos ConfiguratorWorker::getWorkerInfos() {
  this->_workers = getWorkerInfo(this->_jsEnv.cx, this->_jsEnv.global, this->_config);
  return this->_workers;
}
