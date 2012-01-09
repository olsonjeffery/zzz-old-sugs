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

#include "ext.hpp"
namespace fs = boost::filesystem;

static JSBool
filesystem_ls(JSContext* cx, uintN argc, jsval* vp)
{
  JSString* pathStr;
  if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &pathStr)) {
    JS_ReportError(cx, "filesystem_ls: failure to parse path arg");
    return JS_FALSE;
  }
  int numberOfEntries = 1;
  char* pathCharArr = JS_EncodeString(cx, pathStr);

  printf("Provided path: %s\n", pathCharArr);
  fs::path path(pathCharArr);
  int resultLength = 0;
  fs::directory_iterator endIter;

  std::list<std::string> fileList;
  // iterate over dir contents
  if(fs::exists(path) && fs::is_directory(path)) {
    for(fs::directory_iterator dirIter(path); dirIter != endIter; dirIter++) {
      if(fs::is_regular_file(dirIter->status())) {
        resultLength++;
        fileList.push_back(std::string(dirIter->path().c_str()));
      }
    }
  }

  // turn result list in jsval arr
  jsval lsResults[resultLength];
  //lsResults[0] = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, "baz"));
  int resultPosCtr = 0;
  for(std::list<std::string>::iterator iter = fileList.begin(); iter != fileList.end();iter++) {
    JSString* filePathStr = JS_NewStringCopyZ(cx, (*iter).c_str());
    lsResults[resultPosCtr] = STRING_TO_JSVAL(filePathStr);
    resultPosCtr++;
  }

  JSObject* lsArrObj = JS_NewArrayObject(cx, sizeof(lsResults)/sizeof(jsval), lsResults);

  jsval rVal = OBJECT_TO_JSVAL(lsArrObj);
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

static JSFunctionSpec filesystemFuncs[] = {
  JS_FS("ls", filesystem_ls, 1, 0),
  JS_FS_END
};

namespace sugs {
namespace core {
namespace ext {

void FilesystemComponent::registerNativeFunctions(jsEnv jsEnv, sugsConfig config)
{
  JSObject* fsFuncsObj = JS_NewObject(jsEnv.cx, sugs::common::jsutil::getDefaultClassDef(), NULL, NULL);
  if(!JS_DefineFunctions(jsEnv.cx, fsFuncsObj, filesystemFuncs)) {
    JS_ReportError(jsEnv.cx, "failure to define fsFuncs stuff on sugs.funcs");
    JS_ReportPendingException(jsEnv.cx);
    printf("FAILURE TO DEFINE fsFuncs!\n");
  }
  printf("LOADING FILESYSTEM COMPONENT\n");
  sugs::common::jsutil::embedObjectInNamespace(jsEnv.cx, jsEnv.global, jsEnv.global, "sugsNative.core.fs", fsFuncsObj);
}

}}} // namespace sugs::core::fs
