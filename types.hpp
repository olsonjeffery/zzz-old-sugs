#ifndef __types_hpp__
#define __types_hpp__

#include <jsapi.h>
#include <SFML/Graphics.hpp>

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

#endif
