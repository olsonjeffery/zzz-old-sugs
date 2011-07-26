cbody = require 'physics/circularbody'
msgEx = require 'messaging'

return {
  ChipmunkSpace: class
    constructor: (grav) ->
      @_space = __native_chipmunk_new_space grav.x, grav.y
      @imx = new msgEx.InternalMessageExchange()

    step: (stepTime) ->
      @_space.__native_step stepTime

    newCircularBody: (xPos, yPos, mass, radius, friction, groupId) ->
      body = new cbody.CircularBody()

      innerBody = @_space.__native_newCircularBody xPos, yPos, mass, radius, friction, groupId, body
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
      @img.trigger name, msg

    safeTrigger: (name, msg) ->
      @img.safeTrigger name, msg
}
