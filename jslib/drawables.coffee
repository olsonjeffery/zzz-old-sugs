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
    @nativeDrawable = __native_factory_sprite global.sugsConfig.moduleDir + imgPath

# the following are various shape-based primitives
class global.Rectangle extends Drawable
  constructor: (size, fillColor, lineThickness, lineColor) ->
    @nativeDrawable = __native_factory_rectangle size, fillColor, lineThickness, lineColor

class global.Circle extends Drawable
  constructor: (radius, fillColor, lineThickness, lineColor) ->
    @nativeDrawable = __native_factory_circle radius, fillColor, lineThickness, lineColor
