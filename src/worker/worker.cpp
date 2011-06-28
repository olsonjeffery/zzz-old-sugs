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

#include "worker.hpp"

void Worker::initLibraries() {}

void Worker::loadSugsLibraries() {
  predicateResult result;
  // Load up our core dependencies
  result = executeScript("extlibs/js/underscore.js", this->_jsEnv.cx, this->_jsEnv.global);
  if(result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }
  result = executeScript("extlibs/js/coffee-script.js", this->_jsEnv.cx, this->_jsEnv.global);
  if(result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }
  result = executeCoffeeScript("src/sugs.coffee", this->_jsEnv.cx, this->_jsEnv.global);
  if(result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }
  printf(">>> finishing Worker::loadSugsLibraries()\n");
}

void Worker::loadConfig(sugsConfig config) {
  JSObject* sugsConfigObj = JS_NewObject(this->_jsEnv.cx, NULL, NULL, NULL);

  // the members of config that we're converting...
  jsval screenWidthVal = INT_TO_JSVAL(config.screenWidth);
  if(!JS_SetProperty(this->_jsEnv.cx, sugsConfigObj, "screenWidth", &screenWidthVal)) {
    printf("Failed to convert native sugsConfig.screenWidth and store in sugsConfig JSObject\n");
    exit(EXIT_FAILURE);
  }
  jsval screenHeightVal = INT_TO_JSVAL(config.screenHeight);
  if(!JS_SetProperty(this->_jsEnv.cx, sugsConfigObj, "screenHeight", &screenHeightVal)) {
    printf("Failed to convert native sugsConfig.screenHeight and store in sugsConfig JSObject\n");
    exit(EXIT_FAILURE);
  }
  jsval colorDepthVal = INT_TO_JSVAL(config.colorDepth);
  if(!JS_SetProperty(this->_jsEnv.cx, sugsConfigObj, "colorDepth", &colorDepthVal)) {
    printf("Failed to convert native sugsConfig.colorDepth and store in sugsConfig JSObject\n");
    exit(EXIT_FAILURE);
  }
  jsval entryPointIsCoffeeVal = BOOLEAN_TO_JSVAL(config.entryPointIsCoffee);
  if(!JS_SetProperty(this->_jsEnv.cx, sugsConfigObj, "entryPointIsCoffee", &entryPointIsCoffeeVal)) {
    printf("Failed to convert native sugsConfig.entryPointIsCoffee and store in sugsConfig JSObject\n");
    exit(EXIT_FAILURE);
  }
  JSString* moduleDirStr = JS_NewStringCopyN(this->_jsEnv.cx, config.moduleDir, strlen(config.moduleDir));
  jsval moduleDirVal = STRING_TO_JSVAL(moduleDirStr);
  if(!JS_SetProperty(this->_jsEnv.cx, sugsConfigObj, "moduleDir", &moduleDirVal)) {
    printf("Failed to convert native sugsConfig.moduleDir and store in sugsConfig JSObject\n");
    exit(EXIT_FAILURE);
  }
  JSString* moduleEntryPointStr = JS_NewStringCopyN(this->_jsEnv.cx, config.moduleEntryPoint, strlen(config.moduleEntryPoint));
  jsval moduleEntryPointVal = STRING_TO_JSVAL(moduleEntryPointStr);
  if(!JS_SetProperty(this->_jsEnv.cx, sugsConfigObj, "moduleEntryPoint", &moduleEntryPointVal)) {
    printf("Failed to convert native sugsConfig.moduleEntryPoint and store in sugsConfig JSObject\n");
    exit(EXIT_FAILURE);
  }

  jsval sugsConfigVal = OBJECT_TO_JSVAL(sugsConfigObj);
  if(!JS_SetProperty(this->_jsEnv.cx, this->_jsEnv.global, "sugsConfig", &sugsConfigVal)) {
    printf("Failed to convert native sugsConfig and load into global object\n");
    exit(EXIT_FAILURE);
  }
}

void Worker::doWork() { }

void Worker::teardown() {
  teardownContext(this->_jsEnv.cx);
}

void Worker::loadEntryPointScript(const char* entryPoint, bool isCoffee) {
  predicateResult result;
  if (isCoffee) {
    result = executeCoffeeScript(entryPoint, this->_jsEnv.cx, this->_jsEnv.global);
    if(result.result == JS_FALSE) {
      printf(result.message);
      exit(EXIT_FAILURE);
    }
  }
  else {
    result = executeScript(entryPoint, this->_jsEnv.cx, this->_jsEnv.global);
    if(result.result == JS_FALSE) {
      printf(result.message);
      exit(EXIT_FAILURE);
    }
  }
  JSString* epStr = JS_NewStringCopyN(this->_jsEnv.cx, entryPoint, strlen(entryPoint));
  jsval epVal = STRING_TO_JSVAL(epStr);
  jsval argv[1];
  argv[0] = epVal;
  jsval rVal;
  JS_CallFunctionName(this->_jsEnv.cx,this->_jsEnv.global, "addEntryPoint", 1, argv, &rVal);
}
