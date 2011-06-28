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

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <stdio.h>

#include "worker/configurator.hpp"
#include "worker/frontend.hpp"
#include "worker/backend.hpp"

#include "gfx/sfmlsetup.hpp"
#include "gfx/jsgraphics.hpp"
#include "jsinput.hpp"
#include "medialibrary.hpp"

void getConfig(JSRuntime* rt, sugsConfig* config, workerInfos* workers) {
  jsEnv jsEnv = initContext(rt);
  ConfiguratorWorker configurator(rt);
  configurator.initLibraries();
  *config = configurator.getConfig();
  printf("about to parse out workers...\n");
  *workers = configurator.getWorkerInfos();
}

bool appIsClosed = false;
void newBackendWorker(void* wpP) {
  workerPayload* payload = (workerPayload*)wpP;
  JSRuntime* rt = initRuntime(1024L * 1024L * 1024L);
  BackendWorker* worker = new BackendWorker(rt, payload->wi, payload->config);
  worker->initLibraries();
  while(!appIsClosed) {
    worker->doWork();
    sf::Sleep(12);
  }
  delete worker;
  teardownRuntime(rt);
}

int main() {

  /**********************
    ALL THE SETUP STUFF
  **********************/
  predicateResult result;

  printf("setting up runtime and fetching config...\n");
  JSRuntime* rt = initRuntime(1024L * 1024L * 1024L);
  sugsConfig config;
  workerInfos workers;
  getConfig(rt, &config, &workers);

  printf("intializing the worker pool...\n");
  FrontendWorker* frontend = new FrontendWorker(rt, config, workers.frontendWorker);
  frontend->initLibraries();

  sf::Thread* backendThreads[workers.backendsCount];
  for(int ctr = 0; ctr < workers.backendsCount; ctr++) {
    workerPayload* wpP = new workerPayload;
    workerPayload wp = {workers.backendWorkers[ctr], config};
    *wpP = wp;
    backendThreads[ctr] = new sf::Thread(&newBackendWorker, wpP);
    backendThreads[ctr]->Launch();
  }
  /*****************
    MAIN GAME LOOP
  *****************/
  printf("entering main game loop\n");

  while(!frontend->isWindowClosed()) {
    frontend->doWork();
  }

  // signal to the backend threads that it's time to go home
  appIsClosed = true;

  for(int ctr = 0; ctr < workers.backendsCount; ctr++) {
    backendThreads[ctr]->Wait();
    delete backendThreads[ctr];
  }

  /*************
    TEAR DOWN
  *************/
  delete frontend;

  delete workers.backendWorkers;

  shutdownSpidermonkey();
  return EXIT_SUCCESS;
}
