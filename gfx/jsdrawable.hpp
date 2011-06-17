#ifndef __jsdrawables_hpp__
#define __jsdrawables_hpp__

#include <jsapi.h>
#include <SFML/Graphics.hpp>

#include "../common.hpp"

JSClass* getDrawableClassDef();
JSFunctionSpec* getDrawableFunctionSpec();
JSBool getColorFrom(JSContext* cx, JSObject* colorParams, sf::Color* outColor);

#endif
