return {
  CircularBody: class
    constructor: ->

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
      amt += .1
      @_body.__native_applyDirectionalImpulse amt
}
