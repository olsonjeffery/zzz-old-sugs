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

#ifndef __sugs_richclient_gfx_hpp__
#define __sugs_richclient_gfx_hpp__

#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <jsapi.h>

#include "../../sugs-core/common.hpp"
#include "../medialibrary.hpp"

/* pull the graphics natives into the current JSContext */
namespace sugs {
namespace richclient {
namespace gfx {

void registerGraphicsNatives(JSContext* cx, JSObject* global);

struct GraphicsEnv {
  sf::RenderWindow* window;
  JSObject* canvas;
};

// graphics setup stuff
GraphicsEnv initGraphics(JSContext* cx, sugsConfig config);
void teardownGraphics(sf::RenderWindow* window, JSObject* canvas, JSContext* cx);

/* JSCanvas setup */
JSObject* newCanvasFrom(sf::RenderWindow* window, JSContext* cx);

/* JSDrawable */
JSClass* getDrawableClassDef();
JSFunctionSpec* getDrawableFunctionSpec();
JSBool getColorFrom(JSContext* cx, JSObject* colorParams, sf::Color* outColor);

/* SFML setup */

/* JSCircle */
void registerCircleFactory(JSContext* cx, JSObject* global);

/* JSRect */
void registerRectangleFactory(JSContext* cx, JSObject* global);

/* JSSprite */
void registerSpriteFactory(JSContext* cx, JSObject* global);

/* JSText */
void registerTextFactory(JSContext* cx, JSObject* global);

}}} // namespace sugs::richclient::gfx

#endif
