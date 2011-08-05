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

 #include "space.h"
 // private funcs
static JSBool
space_step(JSContext* cx, uintN argc, jsval* vp)
{
  jsdouble stepTime;

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "d", &stepTime)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "space_step: couldn't parse out stepTime arg");
      return JS_FALSE;
  }
  JSObject* spaceObj = JS_THIS_OBJECT(cx, vp);

  cpSpace* space = (cpSpace*)JS_GetPrivate(cx, spaceObj);
  cpSpaceStep(space, stepTime);

  jsval rVal = JSVAL_VOID;
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

static JSBool
space_newCircularBody(JSContext* cx, uintN argc, jsval* vp)
{
  jsdouble xPos;
  jsdouble yPos;
  jsdouble radius;
  jsdouble mass;
  jsdouble friction;
  jsuint groupId;
  jsuint collisionType;
  JSObject* outterJsObj;

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "ddddduuo", &xPos, &yPos, &mass, &radius, &friction, &groupId, &collisionType, &outterJsObj)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "space_newCircularBody: couldn't parse out args");
      return JS_FALSE;
  }
  JSObject* spaceObj = JS_THIS_OBJECT(cx, vp);

  cpSpace* space = (cpSpace*)JS_GetPrivate(cx, spaceObj);

  JSObject* bodyObj = sugs::physics::createNewCircularBodyFrom(cx, space, xPos, yPos, mass, radius, friction, groupId, collisionType, outterJsObj);

  if (bodyObj == NULL)
  {
    // error reason should be reported in createNewCircularBodyFrom()
    return JS_FALSE;
  }

  jsval rVal = OBJECT_TO_JSVAL(bodyObj);
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

static void
removeShapeFromSpaceIterator(cpBody* body, cpShape* shape, void* space)
{
  cpSpaceRemoveShape((cpSpace*)space, shape);
  cpShapeFree(shape);
}
static void
removeConstraintFromSpaceIterator(cpBody* body, cpConstraint* constraint, void* space)
{
  cpSpaceRemoveConstraint((cpSpace*)space, constraint);
  cpConstraintFree(constraint);
}

void
doBodyRemoval(cpSpace* space, cpBody* body) {
  cpBodyEachShape(body, &removeShapeFromSpaceIterator, space);
  cpBodyEachConstraint(body, &removeConstraintFromSpaceIterator, space);
  cpSpaceRemoveBody(space, body);
  cpBodyFree(body);
}

static JSBool
space_removeBody(JSContext* cx, uintN argc, jsval* vp)
{
  JSObject* bodyObj;

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "o", &bodyObj)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "space_removeBody: couldn't parse out args");
      return JS_FALSE;
  }
  JSObject* spaceObj = JS_THIS_OBJECT(cx, vp);

  cpSpace* space = (cpSpace*)JS_GetPrivate(cx, spaceObj);

  cpBody* body = (cpBody*)JS_GetPrivate(cx, bodyObj);

  doBodyRemoval(space, body);

  jsval rVal = JSVAL_VOID;
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

static int
customBeginHandler(cpArbiter *arb, struct cpSpace *space, void *data)
{
  sugs::physics::chipmonkeyData* cD = (sugs::physics::chipmonkeyData*)data;
  jsval argv[5];
  cpBody* bodyA;
  cpBody* bodyB;
  cpArbiterGetBodies(arb, &bodyA, &bodyB);
  cpShape* shapeA;
  cpShape* shapeB;
  cpArbiterGetShapes(arb, &shapeA, &shapeB);
  jsuint ctA = cpShapeGetCollisionType(shapeA);
  jsuint ctB = cpShapeGetCollisionType(shapeB);
  JSObject* bObjA = (JSObject*)cpBodyGetUserData(bodyA);
  JSObject* bObjB = (JSObject*)cpBodyGetUserData(bodyB);
  const char* callbackNameCStr = "onBegin";
  JSString* callbackName = JS_NewStringCopyN(cD->cx, callbackNameCStr, strlen(callbackNameCStr));
  argv[0] = INT_TO_JSVAL(ctA);
  argv[1] = INT_TO_JSVAL(ctB);
  argv[2] = STRING_TO_JSVAL(callbackName);
  argv[3] = OBJECT_TO_JSVAL(bObjA);
  argv[4] = OBJECT_TO_JSVAL(bObjB);
  jsval rVal;
  if(!JS_CallFunctionName(cD->cx, cD->spaceObj, "_callCustomHandlerFor", 5, argv, &rVal)) {
    JS_ReportError(cD->cx, "Exception when executing custom call handler in onBegin..\n");
  }
  return JSVAL_TO_BOOLEAN(rVal);
}

static int
customPreSolveHandler(cpArbiter *arb, struct cpSpace *space, void *data)
{
  sugs::physics::chipmonkeyData* cD = (sugs::physics::chipmonkeyData*)data;
  jsval argv[5];
  cpBody* bodyA;
  cpBody* bodyB;
  cpArbiterGetBodies(arb, &bodyA, &bodyB);
  cpShape* shapeA;
  cpShape* shapeB;
  cpArbiterGetShapes(arb, &shapeA, &shapeB);
  jsuint ctA = cpShapeGetCollisionType(shapeA);
  jsuint ctB = cpShapeGetCollisionType(shapeB);
  JSObject* bObjA = (JSObject*)cpBodyGetUserData(bodyA);
  JSObject* bObjB = (JSObject*)cpBodyGetUserData(bodyB);
  const char* callbackNameCStr = "onPreSolve";
  JSString* callbackName = JS_NewStringCopyN(cD->cx, callbackNameCStr, strlen(callbackNameCStr));
  argv[0] = INT_TO_JSVAL(ctA);
  argv[1] = INT_TO_JSVAL(ctB);
  argv[2] = STRING_TO_JSVAL(callbackName);
  argv[3] = OBJECT_TO_JSVAL(bObjA);
  argv[4] = OBJECT_TO_JSVAL(bObjB);
  jsval rVal;
  if(!JS_CallFunctionName(cD->cx, cD->spaceObj, "_callCustomHandlerFor", 5, argv, &rVal)) {
    JS_ReportError(cD->cx, "Exception when executing custom call handler in onBegin..\n");
  }
  return JSVAL_TO_BOOLEAN(rVal);
}

static void
customPostSolveHandler(cpArbiter *arb, cpSpace *space, void *data)
{
  sugs::physics::chipmonkeyData* cD = (sugs::physics::chipmonkeyData*)data;
  jsval argv[5];
  cpBody* bodyA;
  cpBody* bodyB;
  cpArbiterGetBodies(arb, &bodyA, &bodyB);
  cpShape* shapeA;
  cpShape* shapeB;
  cpArbiterGetShapes(arb, &shapeA, &shapeB);
  jsuint ctA = cpShapeGetCollisionType(shapeA);
  jsuint ctB = cpShapeGetCollisionType(shapeB);
  JSObject* bObjA = (JSObject*)cpBodyGetUserData(bodyA);
  JSObject* bObjB = (JSObject*)cpBodyGetUserData(bodyB);
  const char* callbackNameCStr = "onPostSolve";
  JSString* callbackName = JS_NewStringCopyN(cD->cx, callbackNameCStr, strlen(callbackNameCStr));
  argv[0] = INT_TO_JSVAL(ctA);
  argv[1] = INT_TO_JSVAL(ctB);
  argv[2] = STRING_TO_JSVAL(callbackName);
  argv[3] = OBJECT_TO_JSVAL(bObjA);
  argv[4] = OBJECT_TO_JSVAL(bObjB);
  jsval rVal;
  if(!JS_CallFunctionName(cD->cx, cD->spaceObj, "_callCustomHandlerFor", 5, argv, &rVal)) {
    JS_ReportError(cD->cx, "Exception when executing custom call handler in onBegin..\n");
  }
}

static void
customSeparateHandler(cpArbiter *arb, cpSpace *space, void *data)
{
  sugs::physics::chipmonkeyData* cD = (sugs::physics::chipmonkeyData*)data;
  jsval argv[5];
  cpBody* bodyA;
  cpBody* bodyB;
  cpArbiterGetBodies(arb, &bodyA, &bodyB);
  cpShape* shapeA;
  cpShape* shapeB;
  cpArbiterGetShapes(arb, &shapeA, &shapeB);
  jsuint ctA = cpShapeGetCollisionType(shapeA);
  jsuint ctB = cpShapeGetCollisionType(shapeB);
  JSObject* bObjA = (JSObject*)cpBodyGetUserData(bodyA);
  JSObject* bObjB = (JSObject*)cpBodyGetUserData(bodyB);
  const char* callbackNameCStr = "onSeparate ";
  JSString* callbackName = JS_NewStringCopyN(cD->cx, callbackNameCStr, strlen(callbackNameCStr));
  argv[0] = INT_TO_JSVAL(ctA);
  argv[1] = INT_TO_JSVAL(ctB);
  argv[2] = STRING_TO_JSVAL(callbackName);
  argv[3] = OBJECT_TO_JSVAL(bObjA);
  argv[4] = OBJECT_TO_JSVAL(bObjB);
  jsval rVal;
  if(!JS_CallFunctionName(cD->cx, cD->spaceObj, "_callCustomHandlerFor", 5, argv, &rVal)) {
    JS_ReportError(cD->cx, "Exception when executing custom call handler in onBegin..\n");
  }
}

static JSBool
space_registerCustomCollisionHandlers(JSContext* cx, uintN argc, jsval* vp)
{
  jsuint ctA;
  jsuint ctB;
  JSObject* spaceWrapper;

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "uuo", &ctA, &ctB, &spaceWrapper)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "space_registerCustomCollisionHandlers: couldn't parse out args");
      return JS_FALSE;
  }
  JSObject* spaceObj = JS_THIS_OBJECT(cx, vp);
  cpSpace* space = (cpSpace*)JS_GetPrivate(cx, spaceObj);

  sugs::physics::chipmonkeyData* data = new sugs::physics::chipmonkeyData;
  *data = {cx, spaceWrapper};

  cpSpaceAddCollisionHandler(space, ctA, ctB, customBeginHandler, customPreSolveHandler, customPostSolveHandler, customSeparateHandler, data);

  jsval rVal = JSVAL_VOID;
  JS_SET_RVAL(cx, vp, rVal);
  return JS_TRUE;
}

static JSFunctionSpec
space_native_functions[] = {
  JS_FS("__native_step", space_step, 1, 0),
  JS_FS("__native_newCircularBody", space_newCircularBody, 8, 0),
  JS_FS("__native_removeBody", space_removeBody, 1, 0),
  JS_FS("__native_registerCustomCollisionHandlers", space_registerCustomCollisionHandlers, 2, 0),
  JS_FS_END
};

static void
classdef_space_finalize(JSContext* cx, JSObject* sp) {
  cpSpace* space = (cpSpace*)JS_GetPrivate(cx, sp);
  printf("About to try and delete a cpSpace...\n");
  cpSpaceFree(space);
}

static JSClass
spaceClassDef = {
  "NativeChipmunkSpace",
  JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, classdef_space_finalize
};

static int
defaultBeginHandler(cpArbiter *arb, struct cpSpace *space, void *data)
{
  sugs::physics::chipmonkeyData* cD = (sugs::physics::chipmonkeyData*)data;
  jsval argv[2];
  cpBody* bodyA;
  cpBody* bodyB;
  cpArbiterGetBodies(arb, &bodyA, &bodyB);
  JSObject* bObjA = (JSObject*)cpBodyGetUserData(bodyA);
  JSObject* bObjB = (JSObject*)cpBodyGetUserData(bodyB);
  argv[0] = OBJECT_TO_JSVAL(bObjA);
  argv[1] = OBJECT_TO_JSVAL(bObjB);
  jsval rVal;
  JS_CallFunctionName(cD->cx, cD->spaceObj, "_defaultCollisionBeginHandler", 2, argv, &rVal);
  return JSVAL_TO_BOOLEAN(rVal);
}

static int
defaultPreSolveHandler(cpArbiter *arb, struct cpSpace *space, void *data)
{
  sugs::physics::chipmonkeyData* cD = (sugs::physics::chipmonkeyData*)data;
  jsval argv[2];
  cpBody* bodyA;
  cpBody* bodyB;
  cpArbiterGetBodies(arb, &bodyA, &bodyB);
  JSObject* bObjA = (JSObject*)cpBodyGetUserData(bodyA);
  JSObject* bObjB = (JSObject*)cpBodyGetUserData(bodyB);
  argv[0] = OBJECT_TO_JSVAL(bObjA);
  argv[1] = OBJECT_TO_JSVAL(bObjB);
  jsval rVal;
  JS_CallFunctionName(cD->cx, cD->spaceObj, "_defaultCollisionPreSolveHandler", 2, argv, &rVal);
  return JSVAL_TO_BOOLEAN(rVal);
}

static void
defaultPostSolveHandler(cpArbiter *arb, cpSpace *space, void *data)
{
  sugs::physics::chipmonkeyData* cD = (sugs::physics::chipmonkeyData*)data;
  jsval argv[2];
  cpBody* bodyA;
  cpBody* bodyB;
  cpArbiterGetBodies(arb, &bodyA, &bodyB);
  JSObject* bObjA = (JSObject*)cpBodyGetUserData(bodyA);
  JSObject* bObjB = (JSObject*)cpBodyGetUserData(bodyB);
  argv[0] = OBJECT_TO_JSVAL(bObjA);
  argv[1] = OBJECT_TO_JSVAL(bObjB);
  jsval rVal;
  JS_CallFunctionName(cD->cx, cD->spaceObj, "_defaultCollisionPostSolveHandler", 2, argv, &rVal);
}

static void
defaultSeparateHandler(cpArbiter *arb, cpSpace *space, void *data)
{
  sugs::physics::chipmonkeyData* cD = (sugs::physics::chipmonkeyData*)data;
  jsval argv[2];
  cpBody* bodyA;
  cpBody* bodyB;
  cpArbiterGetBodies(arb, &bodyA, &bodyB);
  JSObject* bObjA = (JSObject*)cpBodyGetUserData(bodyA);
  JSObject* bObjB = (JSObject*)cpBodyGetUserData(bodyB);
  argv[0] = OBJECT_TO_JSVAL(bObjA);
  argv[1] = OBJECT_TO_JSVAL(bObjB);
  jsval rVal;
  JS_CallFunctionName(cD->cx, cD->spaceObj, "_defaultCollisionSeparateHandler", 2, argv, &rVal);
}

/* public funcs */
cpSpace*
sugs::physics::createChipmunkSpaceFrom(int gravX, int gravY)
{
  cpSpace* space = cpSpaceNew();
  cpSpaceSetGravity(space, cpv(gravX, gravY));
}

JSObject*
sugs::physics::newSpaceContainerObject(JSContext* cx, cpSpace* space, JSObject* callingObj)
{
  JSObject* spaceHolder = JS_NewObject(cx, &spaceClassDef, NULL, NULL);
  if (!JS_SetPrivate(cx, spaceHolder, (void*)space)) {
      JS_ReportError(cx, "new_space: couldn't store space pointer");
      return JS_FALSE;
  }
  JS_DefineFunctions(cx, spaceHolder, space_native_functions);

  sugs::physics::chipmonkeyData* data = new chipmonkeyData;
  *data = {cx, callingObj};

  cpSpaceSetDefaultCollisionHandler(space, defaultBeginHandler, defaultPreSolveHandler, defaultPostSolveHandler, defaultSeparateHandler, data);

  return spaceHolder;
}
