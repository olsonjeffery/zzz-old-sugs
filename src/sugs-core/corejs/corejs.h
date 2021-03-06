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

#ifndef __sugs_core_js_hpp__
#define __sugs_core_js_hpp__

#include <jsapi.h>

#include "../common.hpp"
#include "../fs/fs.hpp"

namespace sugs {
namespace core {
namespace js {

// related to running scripts
predicateResult findAndExecuteScript(const char* path, pathStrings paths, JSContext* cx, JSObject* global);
predicateResult executeFullPathJavaScript(const char* path, JSContext* cx, JSObject* global);
predicateResult executeFullPathCoffeeScript(const char* path, JSContext* cx, JSObject* global);
// This is meant to be called with a string literal of JS to get something back from the runtime
predicateResult executeJavascriptSnippet(const char* code, JSContext* cx, JSObject* global);

// javascript system setup
JSRuntime* initRuntime(uint32 maxBytes);
jsEnv initContext(JSRuntime* rt);
void teardownContext(JSContext* cx);
void teardownRuntime(JSRuntime* rt);
void shutdownSpidermonkey();

sugsConfig execConfig(JSContext* cx, JSObject* global);

}}} // namespace sugs::core::js

#endif
