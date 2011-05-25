#ifndef __types_hpp__
#define __types_hpp__

#include <jsapi.h>
#include <SFML/Graphics.hpp>

typedef struct {
  JSRuntime* rt;
  JSContext* cx;
  JSObject* global;
} jsEnv;


typedef struct {
  sf::RenderWindow* window;
} graphicsEnv;

#endif
