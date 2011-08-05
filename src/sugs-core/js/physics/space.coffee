cbody = require 'physics/circularbody'
msgEx = require 'messaging'
mixins = require 'mixins'
patterns = require 'patterns'

return {
  ChipmunkSpace: class
    constructor: (grav) ->
      mixins.into this, patterns.ObjectEventHandlerMixin
      @_space = __native_chipmunk_new_space grav.x, grav.y, this
      @collisionHandlers = {}

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

    collisionHandlers: null
    hasCollisionHandlersFor: (collTypeA, collTypeB) ->
      result = false
      if typeof (@collisionHandlers[collTypeA]) != "undefined"
        if typeof (@collisionHandlers[collTypeB]) != "undefined"
          result = true
      result

    registerCollisionHandlers: (ctA, ctB, handlers) ->
      if typeof(@collisionHandlers[ctA]) == "undefined"
        @collisionHandlers[ctA] = {}
        @collisionHandlers[ctA][ctB] = {}
      else
        if typeof(@collisionHandlers[ctA][ctB]) == "undefined"
          @collisionHandlers[ctB] = {}
      @collisionHandlers[ctA][ctB] = handlers
      @_space.__native_registerCustomCollisionHandlers ctA, ctB, this

    _callCustomHandlerFor: (ctA, ctB, eventName, bodyA, bodyB) ->
      result = true
      if typeof(@collisionHandlers[ctA]) != "undefined"
        if typeof(@collisionHandlers[ctA][ctB]) != "undefined"
          result = @collisionHandlers[ctA][ctB][eventName] bodyA, bodyB
      else
        if typeof(@collisionHandlers[ctB]) != "undefined"
          if typeof(@collisionHandlers[ctB][ctA]) != "undefined"
            result = @collisionHandlers[ctB][ctA][eventName] bodyA, bodyB
      result

    addCollisionHandlersFor: (collTypeA, collTypeB, beginH, preSolveH, postSolveH, separateH) ->
      if not @hasCollisionHandlersFor collTypeA, collTypeB and not @hasCollisionHandlersFor collTypeB, collTypeA
        @registerCollisionHandlers collTypeA, collTypeB,
          onBegin: beginH
          onPreSolve: preSolveH
          onPostSolve: postSolveH
          onSeparate: separateH

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
