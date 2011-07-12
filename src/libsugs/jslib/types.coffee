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

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of Jeffery Olson <olson.jeffery@gmail.com>.
###
return {

  # Canvas class -- The "display surface" that a user draws to during the
  # render loop (callbacks registered to $.render() ). Instances of this
  # class are *only* available during the render loop.
  Canvas: class
    constructor: (nativeCanvas) ->
      @nativeCanvas = nativeCanvas

    draw: (sprite) ->
      @nativeCanvas.__native_draw @nativeCanvas, sprite.nativeDrawable

  # Input class -- provides a means to poll for key/mouse button presses
  # and mouse position. An instance is provided to callbacks registered
  # in $.render(). Instances are *only* available during the render loop.
  Input: class
    constructor: (nativeInput) ->
      @nativeInput = nativeInput

    isKeyDown: (key) ->
      @nativeInput.__native_isKeyDown @nativeInput, key

    getMousePos: ->
      @nativeInput.__native_getMousePos(@nativeInput)
}
