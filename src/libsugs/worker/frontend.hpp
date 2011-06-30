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

#include "worker.hpp"
#include "../common.hpp"

#include "../gfx/sfmlsetup.hpp"
#include "../jsinput.hpp"
#include "../medialibrary.hpp"

class FrontendWorker : public Worker {
  public:
    FrontendWorker(JSRuntime* rt, sugsConfig config, workerInfo worker)
    : Worker(rt)
    {
      this->_config = config;
      printf("initializing graphics.. sw: %d\n", config.screenWidth);
      // init graphics
      this->_gfxEnv = initGraphics(this->_jsEnv.cx, this->_config);
      this->_evEnv = {
        newInputFrom(this->_gfxEnv.window, this->_jsEnv.cx)
      };

      this->_worker = worker;
      this->_isClosed = false;
      // set up graphics libs
      MediaLibrary::RegisterDefaultFont();
    }

    ~FrontendWorker()
    {
      printf("frontend dtor...\n");
      jsval argv[0];
      jsval rVal;
      JS_CallFunctionName(this->_jsEnv.cx,this->_jsEnv.global, "showEntryPoints", 0, argv, &rVal);
      teardownGraphics(this->_gfxEnv.window, this->_gfxEnv.canvas, this->_jsEnv.cx);
    }

    virtual void initLibraries();
    virtual void doWork();
    bool appIsOpen();
    bool isWindowClosed();
  private:
    graphicsEnv _gfxEnv;
    eventEnv _evEnv;
    sugsConfig _config;
    workerInfo _worker;

    bool _isClosed;
};

#endif
