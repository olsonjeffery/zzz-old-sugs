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

#include "jsutil.hpp"

static JSClass
defaultClassDefWithPrivateMember = {
  "NativeDefaultClass",
  JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub
};

JSObject* pullJSObjectFrom(JSContext* cx, JSObject* topLevel, const char* nsPlusPropName)
{
  jsval argv[2];
  argv[0] = OBJECT_TO_JSVAL(topLevel);
  JSString* nsString = JS_NewStringCopyZ(cx, nsPlusPropName);
  argv[1] = STRING_TO_JSVAL(nsString);
  jsval rVal;
  JS_CallFunctionName(cx, topLevel, "pullObjectFrom", 2, argv, &rVal);

  return JSVAL_TO_OBJECT(rVal);
}

namespace sugs {
namespace common {
namespace jsutil {

JSClass* getDefaultClassDef() {
  return &defaultClassDefWithPrivateMember;
}

jsval pullPropertyFromSugsConfigInGlobal(JSContext* cx, JSObject* global, const char* propName) {
  jsval sugsConfigVal;
  if(!JS_GetProperty(cx, global, "sugsConfig", &sugsConfigVal)) {
    printf("unable to pull sugsConfig from global obj\n");
    exit(EXIT_FAILURE);
  }
  JSObject* sugsConfigObj = JSVAL_TO_OBJECT(sugsConfigVal);

  jsval propVal;
  if(!JS_GetProperty(cx, sugsConfigObj, propName, &propVal)) {
    printf("unable to pull prop '%s' from sugsConfig obj\n", propName);
    exit(EXIT_FAILURE);
  }
  return propVal;
}

void embedObjectInNamespaceWithinObject(JSContext* cx, JSObject* global, JSObject* outter, const char* ns, JSObject* inner, const char* propName)
{
  jsval argv[4];
  argv[0] = OBJECT_TO_JSVAL(outter);
  JSString* nsString = JS_NewStringCopyZ(cx, ns);
  argv[1] = STRING_TO_JSVAL(nsString);
  argv[2] = OBJECT_TO_JSVAL(inner);
  JSString* propNameString = JS_NewStringCopyZ(cx, propName);
  argv[3] = STRING_TO_JSVAL(propNameString);
  jsval rVal;
  JS_CallFunctionName(cx, global, "embedObjectInNamespace", 4, argv, &rVal);
}

bool newJSObjectFromFunctionSpec(JSContext* cx, JSFunctionSpec* spec, JSObject** obj)
{
  bool result = true;
  *obj = JS_NewObject(cx, getDefaultClassDef(), NULL, NULL);
  if(!JS_DefineFunctions(cx, *obj, spec)) {
    obj = NULL;
    result = false;
  }

  return result;
}

}}} // namespace sugs::common::jsutil
