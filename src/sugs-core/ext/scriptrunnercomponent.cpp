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

#include "ext.hpp"

namespace sugs {
namespace core {
namespace ext {

predicateResult execStartupCallbacks(jsEnv jsEnv) {
  jsval argv[0];

  jsval rval;
  if (JS_CallFunctionName(jsEnv.cx, jsEnv.global, "doStartup", 0, argv, &rval) == JS_FALSE) {
    return {JS_FALSE, "error occured while called doStartup()\n"};
  }
  return { JS_TRUE, ""};
}

void ScriptRunnerComponent::setup(jsEnv jsEnv, pathStrings paths)
{
  predicateResult result;
  this->loadEntryPointScript(jsEnv, paths, this->_entryPoint.c_str()); // entry point should come from json
}

void ScriptRunnerComponent::loadEntryPointScript(jsEnv jsEnv, pathStrings paths, const char* entryPoint) {
  predicateResult result;
  result = sugs::core::js::findAndExecuteScript(entryPoint, paths, jsEnv.cx, jsEnv.global);
  if(result.result == JS_FALSE) {
    printf(result.message);
    exit(EXIT_FAILURE);
  }

  JSString* epStr = JS_NewStringCopyN(jsEnv.cx, entryPoint, strlen(entryPoint));
  jsval epVal = STRING_TO_JSVAL(epStr);
  jsval argv[1];
  argv[0] = epVal;
  jsval rVal;
}

void callIntoJsMainLoop(jsEnv jsEnv, int msElapsed) {
  jsval argv[1];

  argv[0] = INT_TO_JSVAL(msElapsed);
  jsval rval;
  JS_CallFunctionName(jsEnv.cx, jsEnv.global, "runMainLoop", 1, argv, &rval);
}

bool ScriptRunnerComponent::doWork(jsEnv jsEnv, pathStrings paths) {
  time_t currMs = getCurrentMilliseconds();
  callIntoJsMainLoop(jsEnv, currMs - this->_lastMs);
  this->_lastMs = currMs;
}

Component* ScriptRunnerComponentFactory::create(jsEnv jsEnv, JSObject* configJson)
{
  jsval epVal;
  if (!JS_GetProperty(jsEnv.cx, configJson, "entryPoint", &epVal)) {
    printf("ScriptRunnerComponentFactory::create() : failed to pull entryPoint val from config json object");
    exit(EXIT_FAILURE);
  }
  if (JSVAL_IS_VOID(epVal)) {
    printf("need a valid entryPoint in component config to launch ScriptRunner\n");
    exit(EXIT_FAILURE);
  }
  JSString* epStr = JSVAL_TO_STRING(epVal);
  std::string ep(JS_EncodeString(jsEnv.cx, epStr));

  return new ScriptRunnerComponent(ep);
}

std::string ScriptRunnerComponentFactory::getName()
{
  return "ScriptRunner";
}

}}} // namespace sugs::core::ext
