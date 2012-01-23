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

 #include "sim.h"

JSBool new_space(JSContext* cx, uintN argc, jsval* vp)
{
  jsuint gravX;
  jsuint gravY;
  JSObject* spaceObj;

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "uuo", &gravX, &gravY, &spaceObj)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "new_space: couldn't parse out grav args");
      return JS_FALSE;
  }

  cpSpace* space = sugs::physics::createChipmunkSpaceFrom(gravX, gravY);

  JSObject* spaceHolder = sugs::physics::newSpaceContainerObject(cx, space, spaceObj);

  jsval rVal = OBJECT_TO_JSVAL(spaceHolder);
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

static JSFunctionSpec reformer_chipmunk_native_functions[] = {
  JS_FS("__native_chipmunk_new_space", new_space, 2, 0),
  JS_FS_END
};

namespace sugs {
namespace physics {

void ChipmunkPhysicsComponent::registerNativeFunctions(jsEnv jsEnv, pathStrings paths)
{
  JS_DefineFunctions(jsEnv.cx, jsEnv.global, reformer_chipmunk_native_functions);
}


sugs::core::ext::Component* ChipmunkPhysicsComponentFactory::create(jsEnv jsEnv, JSObject* configJson)
{
  return new sugs::physics::ChipmunkPhysicsComponent();
}

std::string ChipmunkPhysicsComponentFactory::getName()
{
  return "ChipmunkPhysics";
}

}} // namespace sugs::physics
