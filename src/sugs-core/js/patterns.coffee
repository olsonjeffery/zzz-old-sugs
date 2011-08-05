mixins = require 'mixins'
return {
  DeferredActionProcessor : class
    constructor: ->
      @actions = {}
    add: (entId, cb) ->
      if typeof(@actions[entId]) == "undefined"
        @actions[entId] = []
      @actions[entId].push cb
    process: (entId, ent) ->
      puts "CLIENT: flushing deferred actions for #{entId}"
      if typeof(@actions[entId]) != "undefined"
        for cb in @actions[entId]
          cb ent

  ObjectEventHandlerMixin: class extends mixins.MixinBase
    constructor: ->
      @imxClass = class
        constructor: ->
          @handlers = {}

        trigger : (msgId, msg) ->
          matched = @hasHandlerFor msgId
          if not matched
            throw "Unresolved event name #{msgId} trigger'd after startup"
          else
            _.each @handlers[msgId], (cb) -> cb msg

        safeTrigger : (msgId, msg) ->
          matched = @hasHandlerFor msgId
          if matched
            _.each @handlers[msgId], (cb) -> cb msg

        bind : (msgId, callback) ->
          if not @hasHandlerFor msgId
            @handlers[msgId] = []
          @handlers[msgId].push callback

        hasHandlerFor: (msgId) ->
          typeof @handlers[msgId] != "undefined"

    doMix: (receiver) ->
      imx = new @imxClass()

      receiver.__ObjectEventHandlerMixin_imx__ = imx
      receiver.hasHandlerFor = (name) ->
        @__ObjectEventHandlerMixin_imx__.hasHandlerFor name

      receiver.bind = (name, callback) ->
        @__ObjectEventHandlerMixin_imx__.bind name, callback

      receiver.trigger = (name, msg) ->
        @__ObjectEventHandlerMixin_imx__.trigger name, msg

      receiver.safeTrigger = (name, msg) ->
        @__ObjectEventHandlerMixin_imx__.safeTrigger name, msg
}