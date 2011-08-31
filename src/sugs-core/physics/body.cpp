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

 #include "body.h"

 static JSBool
body_getPos(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* bodyObj = JS_THIS_OBJECT(cx, vp);

  cpBody* body = (cpBody*)JS_GetPrivate(cx, bodyObj);

  JSObject* posObj = JS_NewObject(cx, NULL, NULL, NULL);
  cpVect pos = cpBodyGetPos(body);
  jsval xPos = DOUBLE_TO_JSVAL(pos.x);
  jsval yPos = DOUBLE_TO_JSVAL(pos.y);
  if(!JS_SetProperty(cx, posObj, "x", &xPos)) {
     JS_ReportError(cx, "body_getPos: unable to set pos x val");
     return JS_FALSE;
  }
  if(!JS_SetProperty(cx, posObj, "y", &yPos)) {
     JS_ReportError(cx, "body_getPos: unable to set pos y val");
     return JS_FALSE;
  }

  jsval rVal = OBJECT_TO_JSVAL(posObj);
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

struct RadiusFetchingData {
  cpFloat radius;
  unsigned int layerMask;
};

static JSBool
body_getRotation(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* bodyObj = JS_THIS_OBJECT(cx, vp);

  cpBody* body = (cpBody*)JS_GetPrivate(cx, bodyObj);

  jsval rVal = DOUBLE_TO_JSVAL(cpBodyGetAngle(body));
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}
static JSBool
body_setRotation(JSContext* cx, uintN argc, jsval* vp)
{
  jsdouble angleRadiansVal;
  JSObject* spaceObj;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "do", &angleRadiansVal, &spaceObj)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "body_setRotation: couldn't parse out angle");
      return JS_FALSE;
  }

  JSObject* bodyObj = JS_THIS_OBJECT(cx, vp);

  cpBody* body = (cpBody*)JS_GetPrivate(cx, bodyObj);
  cpSpace* space = (cpSpace*)JS_GetPrivate(cx, spaceObj);
  cpBodySetAngle(body, angleRadiansVal);
  cpSpaceReindexShapesForBody(space, body);

  jsval rVal = JSVAL_VOID;
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}
static JSBool
body_applyDirectionalImpulse(JSContext* cx, uintN argc, jsval* vp)
{
  jsdouble amt;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "d", &amt)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "body_applyDirectionalImpulse: couldn't parse out amt");
      return JS_FALSE;
  }

  JSObject* bodyObj = JS_THIS_OBJECT(cx, vp);

  cpBody* body = (cpBody*)JS_GetPrivate(cx, bodyObj);
  cpFloat angle = cpBodyGetAngle(body);
  double xAmt = -sin(angle) * amt;
  double yAmt = cos(angle) * amt;
  cpVect j = {xAmt, yAmt};
  cpBodyApplyImpulse(body, j, cpvzero);

  jsval rVal = JSVAL_VOID;
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

static JSBool
body_setAngVel(JSContext* cx, uintN argc, jsval* vp)
{
  jsdouble angVel;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "d", &angVel)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "body_setAngVel: couldn't parse out angVel");
      return JS_FALSE;
  }

  JSObject* bodyObj = JS_THIS_OBJECT(cx, vp);

  cpBody* body = (cpBody*)JS_GetPrivate(cx, bodyObj);
  cpBodySetAngVel(body, angVel);

  jsval rVal = JSVAL_VOID;
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

static JSBool body_getVelocity(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* bodyObj = JS_THIS_OBJECT(cx, vp);

  cpBody* body = (cpBody*)JS_GetPrivate(cx, bodyObj);

  cpVect velocity = cpBodyGetVel(body);
  jsval xVelVal = DOUBLE_TO_JSVAL(velocity.x);
  jsval yVelVal = DOUBLE_TO_JSVAL(velocity.y);
  JSObject* velocityObj = JS_NewObject(cx, NULL, NULL, NULL);
  if(!JS_SetProperty(cx, velocityObj, "x", &xVelVal)) {
    JS_ReportError(cx, "Failure to set x property of velocity for Body");
  }
  if(!JS_SetProperty(cx, velocityObj, "y", &yVelVal)) {
    JS_ReportError(cx, "Failure to set y property of velocity for Body");
  }
  jsval rVal = OBJECT_TO_JSVAL(velocityObj);
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}
static JSBool body_getVelocityLimit(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* bodyObj = JS_THIS_OBJECT(cx, vp);

  cpBody* body = (cpBody*)JS_GetPrivate(cx, bodyObj);

  jsval rVal = DOUBLE_TO_JSVAL(cpBodyGetVelLimit(body));
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

static JSBool body_setVelocityLimit(JSContext* cx, uintN argc, jsval* vp)
{
  jsdouble velocityVal;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "d", &velocityVal)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "body_setVelocityLimit: couldn't parse out velocity limit");
      return JS_FALSE;
  }

  JSObject* bodyObj = JS_THIS_OBJECT(cx, vp);

  cpBody* body = (cpBody*)JS_GetPrivate(cx, bodyObj);
  cpBodySetVelLimit(body, velocityVal);

  jsval rVal = JSVAL_VOID;
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}
static JSFunctionSpec
body_functionSpec[] = {
  JS_FS("__native_getPos", body_getPos, 0, 0),
  JS_FS("__native_getRotation", body_getRotation, 0, 0),
  JS_FS("__native_setRotation", body_setRotation, 2, 0),
  JS_FS("__native_applyDirectionalImpulse", body_applyDirectionalImpulse, 1, 0),
  JS_FS("__native_setAngVel", body_setAngVel, 1, 0),
  JS_FS("getVelocity", body_getVelocity, 0, 0),
  JS_FS("getVelocityLimit", body_getVelocityLimit, 0, 0),
  JS_FS("setVelocityLimit", body_setVelocityLimit, 1, 0),
  JS_FS_END
};

static void
removeShapeFromBodyIterator(cpBody* body, cpShape* shape, void* data)
{
  printf("removing shape...\n");
  cpShapeFree(shape);
}
static void
removeConstraintFromBodyIterator(cpBody* body, cpConstraint* constraint, void* data)
{
  cpConstraintFree(constraint);
}

void
classdef_circularBody_finalize(JSContext* cx, JSObject* sp) {
  cpBody* body = (cpBody*)JS_GetPrivate(cx, sp);
  printf("About to try and delete a cpBody and, its cpConstraints and its cpShapes...\n");
  cpBodyEachShape(body, &removeShapeFromBodyIterator, 0);
  cpBodyEachConstraint(body, &removeConstraintFromBodyIterator, 0);
  cpBodyFree(body);
}

static JSClass
bodyClassDef = {
  "NativeChipmunkBody",
  JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, classdef_circularBody_finalize
};

namespace sugs {
namespace physics {
JSObject* createNewBodyJsObjectFrom(JSContext* cx, cpSpace* space, cpFloat posX, cpFloat posY, cpFloat mass, cpFloat moment, JSObject* outterJsObj)
{
  cpBody* body = sugs::physics::createNewBodyFrom(space, posX, posY, mass, moment, outterJsObj);
  JSObject* bodyObj = JS_NewObject(cx, &bodyClassDef, NULL, NULL);
  JS_DefineFunctions(cx, bodyObj, body_functionSpec);

  if(!JS_SetPrivate(cx, bodyObj, (void*)body)) {
    JS_ReportError(cx, "createNewBodyJsObjectFrom: Failed to set private for new body..");
    return NULL;
  }

  return bodyObj;
}
cpBody* createNewBodyFrom(cpSpace* space, cpFloat xPos, cpFloat yPos, cpFloat mass, cpFloat moment, JSObject* outterJsObj)
{
  cpBody* circularBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
  cpBodySetPos(circularBody, cpv(xPos, yPos));

  cpBodySetUserData(circularBody, outterJsObj);
  return circularBody;
}

}} // namespace sugs::physics
