###
Copyright 2011 Jeffery Olson <olson.jeffery@gmail.com>. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this list of
    conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice, this list
    of conditions and the following disclaimer in the documentation and/or other materials
    provided with the distribution.

THIS SOFTWARE IS PROVIDED BY JEFFERY OLSON <OLSON.JEFFERY@GMAIL.COM> ``AS IS'' AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
JEFFERY OLSON <OLSON.JEFFERY@GMAIL.COM> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of Jeffery Olson <olson.jeffery@gmail.com>.
###

global = this

# Base class for images that can be drawn to the display
class Drawable
  setPos: (pos) ->
    @nativeDrawable.__native_setPos @nativeDrawable, pos

  getPos: ->
    @nativeDrawable.__native_getPos @nativeDrawable

  getRotation: ->
    @nativeDrawable.__native_getRotation @nativeDrawable

  setRotation: (rot) ->
    @nativeDrawable.__native_setRotation @nativeDrawable, rot

  move: (distances) ->
    @nativeDrawable.__native_move @nativeDrawable, distances

  rotate: (deg) ->
    @nativeDrawable.__native_rotate @nativeDrawable, deg

# Represents a static pixel image
class global.Sprite extends Drawable
  constructor: (imgPath) ->
    @nativeDrawable = __native_factory_sprite imgPath

# the following are various shape-based primitives
class global.Rectangle extends Drawable
  constructor: (size, fillColor, lineThickness, lineColor) ->
    @nativeDrawable = __native_factory_rectangle size, fillColor, lineThickness, lineColor

class global.Circle extends Drawable
  constructor: (radius, fillColor, lineThickness, lineColor) ->
    @nativeDrawable = __native_factory_circle radius, fillColor, lineThickness, lineColor

# Represents some on-screen text
class global.Text extends Drawable
  constructor: (content, fontSize, fontName, colorParams) ->
    puts "provided fontName: #{fontName}"
    @nativeDrawable = __native_factory_text content, fontSize, fontName, colorParams

  setString: (content) ->
    @nativeDrawable.__native_setString @nativeDrawable, content
