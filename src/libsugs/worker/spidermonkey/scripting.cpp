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

#include "scripting.hpp"

/* prototypes */
predicateResult executeScriptFromSrc(const char* path, char** src, int length, JSContext* cx, JSObject* global) {
  printf("executing %s from source!!!\n", path);
  /* Execute a script */
  JSObject *scriptObject;
  jsval rval;

  /* Compile a script file into a script object */
  scriptObject = JS_CompileScript(cx, global, *src, length, path, 1);
  if (!scriptObject) {
    char buffer[2056];
    sprintf(buffer, "Failed to compile %s\n", path);
    return {JS_FALSE, buffer};
  }

  if (JS_AddObjectRoot(cx, &scriptObject) != JS_TRUE) {
    char buffer[2056];
    sprintf(buffer, "Failed to add root object for %s\n", path);
    return {JS_FALSE, buffer};
  }

  /* Execute script object */
  if (JS_ExecuteScript(cx, global, scriptObject, &rval) != JS_TRUE) {
    char buffer[2056];
    sprintf(buffer, "Failed to execute %s\n", path);
    return {JS_FALSE, buffer};
  }

  // done with script..
  if (JS_RemoveObjectRoot(cx, &scriptObject) != JS_TRUE) {
    char buffer[2056];
    sprintf(buffer, "Failed to remove root object for %s\n", path);
    return {JS_FALSE, buffer};
  }

  printf("done executing %s from source...\n", path);
  return {JS_TRUE, ""};
}

predicateResult executeFullPathJavaScript(const char* path, JSContext* cx, JSObject* global) {
  char* srcBuffer;
  int length;
  if (!fileExists(path)) {
    char msg[2056];
    sprintf(msg, "Script %s not found!\n", path);
    return {JS_FALSE, msg};
  }
  readEntireFile(path, &srcBuffer, &length);

  predicateResult result = executeScriptFromSrc(path, &srcBuffer, length, cx, global);
  free(srcBuffer);
  return result;
}

predicateResult executeFullPathCoffeeScript(const char* path, JSContext* cx, JSObject* global) {
  char* buffer;
  int length;
  if (!fileExists(path)) {
    char msg[2056];
    sprintf(msg, "Script %s not found and can't be loaded!\n", path);
    return {JS_FALSE, msg};
  }
  readEntireFile(path, &buffer, &length);

  jsval argv[2];
  JSString* coffeeFileText = JS_NewStringCopyZ(cx, buffer);
  argv[0] = STRING_TO_JSVAL(coffeeFileText);
  JSObject* options = JS_NewObject(cx, NULL, NULL, NULL);
  argv[1] = OBJECT_TO_JSVAL(options);

  jsval csVal;
  if (JS_GetProperty(cx, global, "CoffeeScript", &csVal) != JS_TRUE) {
    return { JS_FALSE, "couldn't get CoffeeScript prop from global\n" };
  }

  JSObject* coffeeScript = JSVAL_TO_OBJECT(csVal);

  JSBool hasCompile;
  JS_HasProperty(cx, coffeeScript, "compile", &hasCompile);
  if (hasCompile != JS_TRUE) {
    printf("barf time?\n", path);
    return {JS_FALSE, "no compile function on CoffeeScript obj. grabbed wrong one?\n"};
  }

  jsval rVal;
  if (JS_CallFunctionName(cx, coffeeScript, "compile", 2, argv, &rVal) != JS_TRUE) {
    char buffer[2056];
    sprintf(buffer, "Failed to compile coffeescript file %s\n", path);
    return {JS_FALSE, buffer};
  }

  JSString* jsSrcString;
  jsSrcString = JSVAL_TO_STRING(rVal);

  char* srcString = JS_EncodeString(cx, jsSrcString);
  if (srcString == NULL) {
    return { JS_FALSE, "failed to encode string when prepping to run coffee file.\n"};
  }

  predicateResult result = executeScriptFromSrc(path, &srcString, strlen(srcString) - sizeof(char), cx, global);
  free(buffer);
  return result;
}

predicateResult findAndExecuteScript(const char* relScriptLoc, pathStrings paths, JSContext* cx, JSObject* global)
{
  bool foundScript = false;
  predicateResult result;
  std::string scriptFragment(relScriptLoc);
  printf("paths length: %d\n", paths.length);
  for(int ctr = 0; foundScript == false && ctr < paths.length;ctr++)
  {
    printf("gonna parse out path.....\n");
    std::string pathPrefix = paths.paths[ctr];
    printf("the dork.. %s\n", pathPrefix.c_str());
    std::string fullPath = pathPrefix + scriptFragment;
    const char* fullPathCStr = fullPath.c_str();
    printf("Looking in %s for %s\n", pathPrefix.c_str(), fullPathCStr);
    if(fileExists(fullPathCStr)) {
      foundScript = true;
      if (doesFilenameEndWithDotCoffee(fullPathCStr)) {
        result = executeFullPathCoffeeScript(fullPathCStr, cx, global);
      }
      else {
        result = executeFullPathJavaScript(fullPathCStr, cx, global);
      }
    }
  }
  if (foundScript == false) {
    char buffer[1028];
    sprintf(buffer, "findAndExecuteScript: FAILed to locate script '%s' in available paths...\n", relScriptLoc);
    result = {JS_FALSE, buffer};
  }
  return result;
}
