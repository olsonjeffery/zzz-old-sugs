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

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> splitAndReturnVector(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

bool embedObjectInNamespace(JSContext* cx, JSObject* outter, const char* ns, JSObject* inner)
{
  bool result = true;
  const std::string fullNsStr(ns);
  if (fullNsStr == "") {
    JS_ReportError(cx, "Must provide a non-empty namespace string");
    return false;
  }
  std::string periodStr = ".";
  char periodChar = periodStr.at(0);
  std::vector<std::string> tokens = splitAndReturnVector(fullNsStr, periodChar);
  if (tokens.size() == 1)
  {
    std::string name = tokens.at(0);
    jsval innerVal = OBJECT_TO_JSVAL(inner);
    if(!JS_SetProperty(cx, outter, name.c_str(), &innerVal)) {
      JS_ReportError(cx, "embedObjectInNamespace: error setting %s property in outter obj", name.c_str());
      result = false;
    }
  }
  else {
    JSObject* current = outter;
    for(int ctr = 0; ctr < tokens.size(); ctr++)
    {
      std::string name = tokens.at(ctr);
      if(ctr == tokens.size() - 1) {
        jsval innerVal = OBJECT_TO_JSVAL(inner);
        if(!JS_SetProperty(cx, current, name.c_str(), &innerVal)) {
          JS_ReportError(cx, "embedObjectInNamespace: failed to set inner property '%s'", ns);
          result = false;
          break;
        }
      }
      else {
        if (name == "") { // skip empty token
          JS_ReportError(cx, "embedObjectInNamespace: empty token in provided namespace '%s'", ns);
          result = false;
          break;
        }
        JSBool hasProp = JS_FALSE;
        if(!JS_HasProperty(cx, current, name.c_str(), &hasProp))
        {
          JS_ReportError(cx, "embedObjectInNamespace: failed to determine if object has property '%s'", name.c_str());
          result = false;
          break;
        }
        if (hasProp == JS_FALSE) {
          JSObject* newProp = JS_NewObject(cx, sugs::common::jsutil::getDefaultClassDef(), NULL, NULL);
          jsval newPropVal = OBJECT_TO_JSVAL(newProp);
          if(!JS_SetProperty(cx, current, name.c_str(), &newPropVal)) {
            JS_ReportError(cx, "embedObjectInNamespace: failed to create new property '%s'", name.c_str());
            result = false;
            break;
          }
        }

        jsval nextPropVal;
        if(!JS_GetProperty(cx, current, name.c_str(), &nextPropVal)) {
          JS_ReportError(cx, "embedObjectInNamespace: failed to get property '%s'", name.c_str());
          result = false;
          break;
        }
        current = JSVAL_TO_OBJECT(nextPropVal);
      }
    }
  }
  return result;
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
