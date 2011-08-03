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
}
