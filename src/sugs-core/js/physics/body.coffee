msgEx = require 'messaging'
mix = require 'mixins'
patterns = require 'patterns'
class Body
    constructor: (@groupId, @collType, @layer) ->
      mix.into this, patterns.ObjectEventHandlerMixin

    setInnerBody: (@_body) ->
    getInnerBody: -> @_body

    getPos: ->
      @_body.__native_getPos()

    getRotation: ->
      @_body.__native_getRotation()

    setRotation: (angle) ->
      @_body.__native_setRotation angle, @_space

    setSpace: (@_space) ->

    applyDirectionalImpulse: (amt) ->
      @_body.__native_applyDirectionalImpulse amt

    setAngVel: (angVel) ->
      @_body.__native_setAngVel angVel, @_space

return Body
