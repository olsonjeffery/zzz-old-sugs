/* Copyright 2011 Jeffery Olson <olson.jeffery@gmail.com>. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this list of
 *     conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice, this list
 *     of conditions and the following disclaimer in the documentation and/or other materials
 *     provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY JEFFERY OLSON <OLSON.JEFFERY@GMAIL.COM> ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * JEFFERY OLSON <OLSON.JEFFERY@GMAIL.COM> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of Jeffery Olson <olson.jeffery@gmail.com>.
 *
 */

#ifndef __worker_hpp__
#define __worker_hpp__

#include <SFML/System.hpp>
#include <list>

#include <jsapi.h>

#include "../ext/ext.hpp"
#include "../common.hpp"
#include "../messaging/messageexchange.hpp"

#include "../corejs/corejs.h"

namespace sugs {
namespace core {
namespace worker {

class Worker
{
  public:
    // for use by child implementations..
    // if you're using this ctor, you'll prolly need to
    // override init(), as well
    Worker(JSRuntime* rt, MessageExchange* msgEx, std::string prefix)
    {
      this->_jsEnv = sugs::core::js::initContext(rt);
      if (msgEx != NULL) {
        this->_msgEx = msgEx;
        this->_workerId = this->_msgEx->registerNewWorker(prefix);
      }
    }

    Worker(MessageExchange* msgEx, std::string prefix, pathStrings paths, std::string dataJson)
    {
      if (msgEx != NULL) {
        this->_msgEx = msgEx;
        this->_workerId = this->_msgEx->registerNewWorker(prefix);
      }
      this->_receivedKillSignal = false;
      this->_paths = paths;
      this->_dataJson = dataJson;
    }

    ~Worker() {
      jsval argv[0];
      jsval rVal;
      JS_CallFunctionName(this->_jsEnv.cx,this->_jsEnv.global, "showEntryPoints", 0, argv, &rVal);
      sugs::core::js::teardownContext(this->_jsEnv.cx);
      sugs::core::js::teardownRuntime(this->_jsEnv.rt);
      std::cout << "FINISHING DTOR FOR " << this->_workerId << std::endl;
    }

    void addComponentPair(sugs::core::ext::ComponentPair pair);

    void start(bool runInNewThread);
    virtual void init();
    virtual void begin();
    void kill();
    virtual void teardown();

    MessageExchange* getMessageExchange();
    pathStrings getPaths();
    std::string getWorkerId();
    JSObject* getData();
    bool runningInNewThread();

    bool receivedKillSignal();
  protected:
    void addComponent(sugs::core::ext::Component* c);
    bool _receivedKillSignal;
    void loadComponents(pathStrings paths);
    void loadSugsLibraries(pathStrings paths);
    void loadConfig(pathStrings paths, std::string dataJsonStr);
    void setupEnvironment();
    void processPendingMessages();
    jsEnv _jsEnv;
    MessageExchange* _msgEx;
    std::string _workerId;
    std::list<sugs::core::ext::Component*> _components;
    std::list<sugs::core::ext::ComponentPair> _componentPairs;

    pathStrings _paths;
    std::string _dataJson;
    JSObject* _dataObj;

    bool _runInNewThread;
    sf::Thread* _workerThread;
};

class ConfiguratorWorker : public Worker
{
  public:
    ConfiguratorWorker(JSRuntime* rt)
    : Worker(rt, NULL, "configurator")
    {
      std::string noPaths[0];

      this->parseConfigFile();
      this->_paths = this->_config.paths;
    }

    ~ConfiguratorWorker()
    {
      printf("configurator dtor...\n");
    }
    sugsConfig getConfig();
    virtual void init();
  private:
    void parseConfigFile();
    sugsConfig _config;
    workerInfos _workers;
    std::string _pathToExtLibs;
};

typedef struct {
  Worker* worker;
} contextPrivateData;

}}} // namespace sugs::core::worker

#endif
