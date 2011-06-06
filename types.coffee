global = this

# Sprite class -- Encapsulates a loaded image and it's position on
# the screen
class Sprite
  constructor: (imgPath) ->
    @nativeSprite = __native_newSprite(global.__modDir + imgPath)

  setPos: (pos) ->
    @nativeSprite.__native_setPos @nativeSprite, pos

  getPos: ->
    @nativeSprite.__native_getPos @nativeSprite

  move: (distances) ->
    @nativeSprite.__native_move @nativeSprite, distances
global.Sprite = Sprite

# Canvas class -- The "display surface" that a user draws to during the
# render loop (callbacks registered to $.render() ). Instances of this
# class are *only* available during the render loop.
class Canvas
  constructor: (nativeCanvas) ->
    @nativeCanvas = nativeCanvas

  draw: (sprite) ->
    @nativeCanvas.__native_draw @nativeCanvas, sprite.nativeSprite
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
