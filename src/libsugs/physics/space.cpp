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
  JSObject* outterJsObj;

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "dddddo", &xPos, &yPos, &mass, &radius, &friction, &groupId, &outterJsObj)) {
      /* Throw a JavaScript exception. */
      JS_ReportError(cx, "space_newCircularBody: couldn't parse out args");
      return JS_FALSE;
  }
  JSObject* spaceObj = JS_THIS_OBJECT(cx, vp);

  cpSpace* space = (cpSpace*)JS_GetPrivate(cx, spaceObj);

  JSObject* bodyObj = sugs::physics::createNewCircularBodyFrom(cx, space, xPos, yPos, mass, radius, friction, groupId, outterJsObj);

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

static JSFunctionSpec
space_native_functions[] = {
  JS_FS("__native_step", space_step, 1, 0),
  JS_FS("__native_newCircularBody", space_newCircularBody, 7, 0),
  JS_FS("__native_removeBody", space_removeBody, 1, 0),
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

/* public funcs */
cpSpace*
sugs::physics::createChipmunkSpaceFrom(int gravX, int gravY)
{
  cpSpace* space = cpSpaceNew();
  cpSpaceSetGravity(space, cpv(gravX, gravY));
}

JSObject*
sugs::physics::newSpaceContainerObject(JSContext* cx, cpSpace* space)
{
  JSObject* spaceHolder = JS_NewObject(cx, &spaceClassDef, NULL, NULL);
  if (!JS_SetPrivate(cx, spaceHolder, (void*)space)) {
      JS_ReportError(cx, "new_space: couldn't store space pointer");
      return JS_FALSE;
  }
  JS_DefineFunctions(cx, spaceHolder, space_native_functions);

  return spaceHolder;
}
