#ifndef __types_hpp__
#define __types_hpp__

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

/* util functions */
void readEntireFile(const char* path, char** buffer, int* length);
bool fileExists(const char * filename);

#endif
