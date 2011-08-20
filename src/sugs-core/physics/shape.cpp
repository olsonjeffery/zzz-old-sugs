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

#include "shape.h"

static JSBool
circleShape_getRadius(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* shapeObj = JS_THIS_OBJECT(cx, vp);

  cpShape* shape = (cpShape*)JS_GetPrivate(cx, shapeObj);

  cpFloat radius = cpCircleShapeGetRadius(shape);
  jsval rVal = DOUBLE_TO_JSVAL(radius);
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

static JSFunctionSpec
circleShape_functionSpec[] = {
  JS_FS("__native_getRadius", circleShape_getRadius, 0, 0),
  JS_FS_END
};

static JSClass
circleShapeClassDef = {
  "NativeChipmunkCircleShape",
  JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub
};

namespace sugs {
namespace physics {

JSObject* createNewCircleShapeJSObjectFor(JSContext* cx, cpSpace* space, cpBody* body, cpFloat radius, cpFloat friction, cpVect offset,
                                                unsigned int groupId, unsigned int collisionType, unsigned int layers)
{
  cpShape* circleShape = sugs::physics::createNewCircleShapeFor(space, body, radius, friction, offset, groupId, collisionType, layers);
  if (circleShape == NULL)
  {
    // error reason should be reported in createNewCircularBodyFrom()
    return JS_FALSE;
  }

  JSObject* circleShapeObj = JS_NewObject(cx, &circleShapeClassDef, NULL, NULL);
  if(!JS_DefineFunctions(cx, circleShapeObj, circleShape_functionSpec)) {
    JS_ReportError(cx, "space_newCircleShape: unable to define funcs.");
    return NULL;
  }
  if(!JS_SetPrivate(cx, circleShapeObj, circleShape)) {
    JS_ReportError(cx, "space_newCircleShape: unable to store cpShape* in circleShapeObj private slot");
    return NULL;
  }
  return circleShapeObj;
}

cpShape* createNewCircleShapeFor(cpSpace* space, cpBody* body, cpFloat radius, cpFloat friction, cpVect offset,
                                                unsigned int groupId, unsigned int collisionType, unsigned int layers) {
  cpShape* circularShape = cpSpaceAddShape(space, cpCircleShapeNew(body, radius, offset));
  cpShapeSetFriction(circularShape, friction);
  cpShapeSetGroup(circularShape, groupId);
  cpShapeSetCollisionType(circularShape, collisionType);
  cpShapeSetLayers(circularShape, layers);
  return circularShape;
}

}} // namespace sugs::physics
