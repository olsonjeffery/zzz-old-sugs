global = this

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

# Sprite class -- Encapsulates a loaded image and it's position on
# the screen
class global.Sprite extends Drawable
  constructor: (imgPath) ->
    @nativeDrawable = __native_factory_sprite global.sugsConfig.moduleDir + imgPath

class global.Rectangle extends Drawable
  constructor: (size, fillColor, lineThickness, lineColor) ->
    @nativeDrawable = __native_factory_rectangle size, fillColor, lineThickness, lineColor

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
