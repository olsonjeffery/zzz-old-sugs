#include "worker.hpp"

namespace sugs {
namespace core {
namespace worker {

void ConfiguratorWorker::parseConfigFile() {
  this->_config = sugs::core::js::execConfig(this->_jsEnv.cx, this->_jsEnv.global);
  this->_paths = this->_config.paths;
}

void ConfiguratorWorker::init() {
  this->loadConfig(this->_paths, "{}");
  this->loadSugsLibraries(this->_paths);
}

sugsConfig ConfiguratorWorker::getConfig() {
  return this->_config;
}

}}} // namespace sugs::core::worker
