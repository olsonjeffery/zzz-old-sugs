global = this

# Canvas class -- The "display surface" that a user draws to during the
# render loop (callbacks registered to $.render() ). Instances of this
# class are *only* available during the render loop.
class Canvas
  constructor: (nativeCanvas) ->
    @nativeCanvas = nativeCanvas

  draw: (sprite) ->
    @nativeCanvas.__native_draw @nativeCanvas, sprite.nativeDrawable
global.Canvas = Canvas

# Input class -- provides a means to poll for key/mouse button presses
# and mouse position. An instance is provided to callbacks registered
# in $.render(). Instances are *only* available during the render loop.
class Input
  constructor: (nativeInput) ->
    @nativeInput = nativeInput

  isKeyDown: (key) ->
    @nativeInput.__native_isKeyDown @nativeInput, key
global.Input = Input
