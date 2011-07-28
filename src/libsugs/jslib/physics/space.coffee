cbody = require 'physics/circularbody'
msgEx = require 'messaging'

return {
  ChipmunkSpace: class
    constructor: (grav) ->
      @_space = __native_chipmunk_new_space grav.x, grav.y, this
      @imx = new msgEx.InternalMessageExchange()

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

    hasHandlerFor: (name) ->
      @imx.hasHandlerFor name

    bind: (name, callback) ->
      @imx.bind name, callback

    trigger: (name, msg) ->
      @imx.trigger name, msg

    safeTrigger: (name, msg) ->
      @imx.safeTrigger name, msg

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
