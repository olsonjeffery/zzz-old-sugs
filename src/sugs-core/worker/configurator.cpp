#include "worker.hpp"

namespace sugs {
namespace core {
namespace worker {

void ConfiguratorWorker::parseConfigFile() {
  this->_config = sugs::core::js::execConfig(this->_jsEnv.cx, this->_jsEnv.global);
}

void ConfiguratorWorker::initLibraries() {
  this->loadConfig(this->_config);
  this->loadSugsLibraries(this->_config.paths);
}

sugsConfig ConfiguratorWorker::getConfig() {
  return this->_config;
}

}}} // namespace sugs::core::worker
