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

#include <list>

#include <jsapi.h>

#include "../ext/component.h"
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

    Worker(MessageExchange* msgEx, std::string prefix, sugsConfig config, std::string entryPoint)
    {
      if (msgEx != NULL) {
        this->_msgEx = msgEx;
        this->_workerId = this->_msgEx->registerNewWorker(prefix);
      }
      this->_receivedKillSignal = false;
      this->_entryPoint = entryPoint;
      this->_config = config;
      this->_lastMs = getCurrentMilliseconds();
    }

    ~Worker() {
      jsval argv[0];
      jsval rVal;
      JS_CallFunctionName(this->_jsEnv.cx,this->_jsEnv.global, "showEntryPoints", 0, argv, &rVal);
      sugs::core::js::teardownContext(this->_jsEnv.cx);
      sugs::core::js::teardownRuntime(this->_jsEnv.rt);
    }

    void addComponent(sugs::ext::Component* c);
    virtual void init();
    virtual void teardown();
    virtual void begin();
    void kill();

    MessageExchange* getMessageExchange();

    bool receivedKillSignal();
  protected:
    bool _receivedKillSignal;
    void loadComponents(sugsConfig config);
    void loadSugsLibraries(pathStrings paths);
    void loadConfig(sugsConfig config);
    void loadEntryPointScript(const char* path, pathStrings paths);
    void processPendingMessages();
    jsEnv _jsEnv;
    MessageExchange* _msgEx;
    std::string _workerId;
    std::list<sugs::ext::Component*> _components;
  private:
    std::string _entryPoint;
    sugsConfig _config;
    clock_t _lastMs;
};

class ConfiguratorWorker : public Worker
{
  public:
    ConfiguratorWorker(JSRuntime* rt)
    : Worker(rt, NULL, "")
    {
      this->parseConfigFile();
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

}}} // namespace sugs::core::worker

#endif
