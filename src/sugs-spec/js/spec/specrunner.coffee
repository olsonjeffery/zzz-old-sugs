pullMessageFrom = (err) ->
  if typeof(err) == 'string'
    err
  else
    err.message

class SpecFrameworkRunner
  constructor: (@cliReporter, @ctxClass, @specClass) ->
    @contexts = []
    @beforeAlls = []
    @afterAlls = []
    @contextStack = []

    @results =
      successes: 0
      totalSpecs: 0
      totalContexts: 0
      failures: 0
      contextFailures: 0
      errors: 0
      unimpl: 0

  run: ->
    puts "Running specs..."
    puts ""
    for cb in @beforeAlls
      cb()
    for ctx in @contexts
      if ctx.result == 'fail'
        ctx.failWith ctx.failureMessage
      else
        ctx.run()
    for cb in @afterAlls
      cb()
    return @results

  addContext: (contextName, handler) ->
    isChildCtx = @contextStack.length > 0
    ctx = new @ctxClass contextName, @results, @cliReporter
    if isChildCtx
      parentCtx = @contextStack.last()
      parentCtx.children.push ctx
    else
      @contexts.push ctx
    @contextStack.push ctx
    try
      handler()
    catch error
      msg = pullMessageFrom error
      ctx.failureMessage = msg
      ctx.result = 'fail'
    @contextStack.pop()

  addSpecification: (specName, handler) ->
    specification = new @specClass specName, handler, @results, @cliReporter
    currCtx = @contextStack.last()
    currCtx.specs.push specification

return SpecFrameworkRunner