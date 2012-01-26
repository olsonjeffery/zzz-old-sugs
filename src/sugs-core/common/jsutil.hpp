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

 #ifndef __jsutil_hpp__
 #define __jsutil_hpp__

 #include <jsapi.h>

namespace sugs {
namespace common {
namespace jsutil {

JSClass* getDefaultClassDef();

// sugs::common::jsutil::embedObjectInNamespace() -- given a valid outerObj, a period-delimited string representing
// a nested "namespace" within said outerObj, and an addedObj to be inserted, place the addedObj within the outerObj,
// created "stub" objects as we go.
//
// params:
// JSContext* cx -- current JS context
// JSObject* global -- global JS obj for this context. used to access JS impl.
// JSObject* outerObj -- the outer JS object that is being added to
// const char* ns -- period-delimited string indicating the nested namespace being inserted into. If the object doesn't
//   exist already, it will be created as we go.
// JSObject* addedObj -- the object being added to the outerObj
void embedObjectInNamespace(JSContext* cx, JSObject* global, JSObject* outter, const char* ns, JSObject* inner);

bool newJSObjectFromFunctionSpec(JSContext* cx, JSFunctionSpec* spec, JSObject** out);
}
}
}

 #endif
