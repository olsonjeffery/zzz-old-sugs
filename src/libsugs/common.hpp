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

#ifndef __common_hpp__
#define __common_hpp__

#include <stdio.h>
#include <stdlib.h>
#include <jsapi.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <time.h>

#include "common/rng.hpp"

typedef struct {
  JSIntn result;
  char* message;
} predicateResult;

typedef struct {
  JSRuntime* rt;
  JSContext* cx;
  JSObject* global;
} jsEnv;

typedef struct {
  sf::RenderWindow* window;
  JSObject* canvas;
} graphicsEnv;

typedef struct {
  JSObject* input;
} eventEnv;

typedef struct {
  char* moduleDir;
  char* moduleEntryPoint;
  int screenWidth;
  int screenHeight;
  int colorDepth;
  JSBool entryPointIsCoffee;
} sugsConfig;

typedef struct {
  char* entryPoint;
  JSBool isCoffee;
} workerInfo;

typedef struct {
  workerInfo* backendWorkers;
  int backendsCount;
  workerInfo frontendWorker;
} workerInfos;

typedef struct {
  workerInfo wi;
  sugsConfig config;
} workerPayload;

/* util functions */
void readEntireFile(const char* path, char** outBuffer, int* outLength);
bool fileExists(const char * filename);
bool doesFilenameEndWithDotCoffee(const char* filename);
clock_t getCurrentMilliseconds();

#define SUGS_JSVAL_TO_NUMBER(n) JSVAL_IS_INT(n) ? JSVAL_TO_INT(n): JSVAL_TO_DOUBLE(n)

#endif