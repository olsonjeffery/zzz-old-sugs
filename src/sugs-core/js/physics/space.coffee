cbody = require 'physics/circularbody'
msgEx = require 'messaging'
mixins = require 'mixins'
patterns = require 'patterns'

return {
  ChipmunkSpace: class
    constructor: (grav) ->
      mixins.into this, patterns.ObjectEventHandlerMixin
      @_space = __native_chipmunk_new_space grav.x, grav.y, this

    step: (stepTime) ->
      @_space.__native_step stepTime

    newCircularBody: (xPos, yPos, mass, radius, friction, groupId, collType) ->
      body = new cbody.CircularBody groupId, collType

      innerBody = @_space.__native_newCircularBody xPos, yPos, mass, radius, friction, groupId, collType, body
      body.setInnerBody innerBody
      body.setSpace @_space
      body

    removeBody: (body) ->
      @_space.__native_removeBody body._body

    _defaultCollisionBeginHandler: ->
      true
    onCollBegin: (f) ->
      @_defaultCollisionBeginHandler = f

    _defaultCollisionPreSolveHandler: ->
      true
    onCollPre: (f) ->
      @_defaultCollisionPreSolveHandler = f

    _defaultCollisionPostSolveHandler: ->
    onCollPost: (f) ->
      @_defaultCollisionPostSolveHandler = f

    _defaultCollisionSeparateHandler: ->
    onCollSeparate: (f) ->
      @_defaultCollisionSeparateHandler = f
}
