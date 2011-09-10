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
 * JEFFERY OLSON <OLSON.JEFFERY@GMAI`L.COM> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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
#include "speccomponent.h"

static JSFunctionSpec specFuncs[] = {
  JS_FS_END
};

void storeSpecRunnerInputPath(jsEnv jsEnv, std::string rawPaths)
{
  const char* pathCStr = rawPaths.c_str();
  JSString* pathString = JS_NewStringCopyZ(jsEnv.cx, pathCStr);
  jsval pathVal = STRING_TO_JSVAL(pathString);
  JSObject* specObj = JS_NewObject(jsEnv.cx, sugs::common::jsutil::getDefaultClassDef(), NULL, NULL);
  if(!JS_SetProperty(jsEnv.cx, specObj, "rawPath", &pathVal)) {
    JS_ReportError(jsEnv.cx, "storeSpecRunnerInputPath: failed to set rawPAth prop");
    JS_ReportPendingException(jsEnv.cx);
  }
  sugs::common::jsutil::embedObjectInNamespaceWithinObject(jsEnv.cx, jsEnv.global, jsEnv.global, "", specObj, "spec");
  printf("storing the rawPath! %s\n", rawPaths.c_str());
}

namespace sugs {
namespace spec {

void SpecComponent::registerNativeFunctions(jsEnv jsEnv, sugsConfig config)
{
  std::string rawPaths = this->_rawPaths;
  storeSpecRunnerInputPath(jsEnv, rawPaths);
}

void SpecComponent::doWork(jsEnv jsEnv, sugsConfig config)
{
}

}} // namespace sugs::spec
