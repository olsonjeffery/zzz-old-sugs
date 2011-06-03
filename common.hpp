#ifndef __common_hpp__
#define __common_hpp__

#include <stdio.h>
#include <stdlib.h>
#include <jsapi.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>

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

/* util functions */
void readEntireFile(const char* path, char** buffer, int* length);
bool fileExists(const char * filename);

#endif
