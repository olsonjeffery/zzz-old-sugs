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

#ifndef __frontend_worker_hpp__
#define __frontend_worker_hpp__

#include <jsapi.h>
#include <SFML/Graphics.hpp>

#include "../sugs-core/worker/worker.hpp"
#include "../sugs-core/common.hpp"
#include "../sugs-core/common/jsutil.hpp"

#include "gfx/gfx.hpp"
#include "jsinput.hpp"
#include "medialibrary.hpp"

class FrontendWorker : public Worker {
  public:
    FrontendWorker(JSRuntime* rt, sugsConfig config, std::string entryPoint, MessageExchange* msgEx)
    : Worker(rt, msgEx, "frontend")
    {
      this->_config = config;

      this->componentSetup(this->_jsEnv, this->_config);

      this->_entryPoint = entryPoint;
      this->_isClosed = false;
    }

    ~FrontendWorker()
    {
      this->componentTeardown(this->_jsEnv);
    }

    void componentDoWork(jsEnv jsEnv);
    void componentSetup(jsEnv jsEnv, sugsConfig config);
    void componentTeardown(jsEnv jsEnv);
    void componentRegisterNativeFunctions(jsEnv jsEnv, sugsConfig config);

    virtual void initLibraries();
    virtual void doWork();
    bool appIsOpen();
    void closeApp();
    bool isWindowClosed();
  private:
    sugs::richclient::gfx::GraphicsEnv _gfxEnv;
    sugsConfig _config;
    std::string _entryPoint;

    bool _isClosed;
};

#endif
