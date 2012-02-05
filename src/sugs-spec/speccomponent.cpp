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
#include "speccomponent.hpp"

static JSBool
spec_runScript(JSContext* cx, uintN argc, jsval* vp)
{
  JSString* pathStrObj;
  if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &pathStrObj)) {
    JS_ReportError(cx, "filesystem_ls: failure to parse path arg");
    return JS_FALSE;
  }
  char* pathStr = JS_EncodeString(cx, pathStrObj);
  JSObject* global = JS_GetGlobalObject(cx);
  predicateResult result;
  if (sugs::core::fs::fileExists(pathStr)) {
    bool isCoffee = sugs::core::fs::doesFilenameEndWithDotCoffee(pathStr);
    if (isCoffee)
    {
      result = sugs::core::js::executeFullPathCoffeeScript(pathStr, cx, global);
    }
    else
    {
      result = sugs::core::js::executeFullPathJavaScript(pathStr, cx, global);
    }
  }
  if (!result.result) {
    JS_ReportPendingException(cx);
    return JS_FALSE;
  }
  jsval rVal = JSVAL_VOID;
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}
static JSFunctionSpec specFuncs[] = {
  JS_FS("runScript", spec_runScript, 1, 0),
  JS_FS_END
};

namespace sugs {
namespace spec {

void SpecComponent::setup(jsEnv jsEnv, pathStrings paths)
{
  JSObject* specObj;
  if(!sugs::common::jsutil::newJSObjectFromFunctionSpec(jsEnv.cx, specFuncs, &specObj)) {
    printf("failure to register spec global natives!\n");
    exit(1);
  }
  sugs::common::jsutil::embedObjectInNamespace(jsEnv.cx, jsEnv.global, "sugs.api.spec", specObj);
}

sugs::core::ext::Component* SpecComponentFactory::create(jsEnv jsEnv, JSObject* configJson)
{
  return new SpecComponent();
}

std::string SpecComponentFactory::getName()
{
  return "Spec";
}

}} // namespace sugs::spec
