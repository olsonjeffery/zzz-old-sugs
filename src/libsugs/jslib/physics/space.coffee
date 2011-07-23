cbody = require 'physics/circularbody'

return {
  ChipmunkSpace: class
    constructor: (grav) ->
      @_space = __native_chipmunk_new_space grav.x, grav.y

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
}
