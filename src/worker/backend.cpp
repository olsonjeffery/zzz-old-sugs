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

#include "backend.hpp"

void BackendWorker::initLibraries() {
  // load core libs
  printf("INSIDE BACKEND loadSugsLibraries()...\n");
  this->loadSugsLibraries();
  this->loadConfig(this->_config);

  printf("RUNNING BACKEND ENTRY POINT: %s is coffee? %s\n", this->_worker.entryPoint, this->_worker.isCoffee == JS_TRUE ? "yes" : "no");
  predicateResult result;
  this->loadEntryPointScript(_worker.entryPoint, _worker.isCoffee);

  printf("PROCSESING BACKEND STARTUP\n");
  // run $.startup() in user code
  result = execStartupCallbacks(this->_jsEnv);
  if (result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }
  printf("DONE PROCSESING BACKEND STARTUP\n");
}

void callIntoJsMainLoop(jsEnv jsEnv, int msElapsed) {
  jsval argv[1];

  argv[0] = INT_TO_JSVAL(msElapsed);
  jsval rval;
  //printf("before calling runMainLoop..\n");
  JS_CallFunctionName(jsEnv.cx, jsEnv.global, "runMainLoop", 1, argv, &rval);
  //printf("after calling runMainLoop..\n");
}

void BackendWorker::doWork() {
  //clock_t currMs = getCurrentMilliseconds();
  clock_t msElapsed = 12;//currMs - this->_lastMs;
  callIntoJsMainLoop(this->_jsEnv, msElapsed);
  //this->_lastMs = currMs;
}
