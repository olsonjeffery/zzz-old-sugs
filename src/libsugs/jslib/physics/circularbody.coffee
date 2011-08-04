msgEx = require 'messaging'
mixins = require 'mixins'
patterns = require 'patterns'
return {
  CircularBody: class
    constructor: (@groupId, @collType) ->
      mixins.into this, patterns.ObjectEventHandlerMixin

    setInnerBody: (@_body) ->

    getPos: ->
      rawPos = @_body.__native_getPos()
      return {
        x: rawPos.x
        y: -rawPos.y
      }

    getRadius: ->
      @_body.__native_getRadius()

    getRotation: ->
      @_body.__native_getRotation()

    setRotation: (angle) ->
      @_body.__native_setRotation angle, @_space

    setSpace: (@_space) ->

    applyDirectionalImpulse: (amt) ->
      @_body.__native_applyDirectionalImpulse amt
}
