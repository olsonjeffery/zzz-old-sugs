{ assert, AssertionError } = require 'spec/assert'

results =
  totalSpecs: 0
  totalContexts: 0
  failures: 0
  errors: 0
  unimpl: 0

class Specification
  constructor: (@name, @handler) ->
    @pass = 'undefined'
    @failureMessage = ''

  run: ->
    results.totalSpecs++
    if typeof(@handler) != 'undefined'
      try
        @handler()
      catch e
        if e not instanceof AssertionError
          results.errors++
          @pass = 'error'
          if typeof(e) == 'string'
            @failureMessage = "    ERROR: #{e}"
          else
            @failureMessage = "    ERROR: #{e.message}"
        else
          results.failures++
          @pass = 'fail'
          @failureMessage = e.message
      if @pass == 'undefined'
        @pass = 'success'
    else
      results.unimpl++

  displayResults: ->
    suffix = ''
    if @pass == 'fail'
      suffix = ' [FAILED]'
    if @pass == 'error'
      suffix = ' [ERROR]'
    if @pass == 'undefined'
      suffix = ' [UNIMPLEMENTED]'
    puts "  It #{@name}#{suffix}"
    if @pass == 'fail' or @pass == 'error'
      puts @failureMessage

class SpecContext
  constructor: (@name) ->
    @befores = []
    @specs = []
    @afters = []
    @children = []

  hasFailingSpecs: ->
    result = false
    for spec in @specs
      if spec.pass == 'fail'
        result = true
        break
    result

  displaySpecResults: ->
    for spec in @specs
      spec.displayResults()

  run: ->
    results.totalContexts++
    for cb in @befores
      cb()
    for spec in @specs
      spec.run()
    failed = ''
    if @hasFailingSpecs()
      failed = " [FAILED]"
    puts "When #{@name}#{failed}"
    @displaySpecResults()
    puts ""
    for ctx in @children
      ctx.run()
    for cb in @afters
      cb()


class SpecFrameworkRunner
  constructor: ->
    @contexts = []
    @beforeAlls = []
    @afterAlls = []
    @contextStack = []

  run: ->
    puts "Running specs..."
    puts ""
    for cb in @beforeAlls
      cb()
    for ctx in @contexts
      ctx.run()
    for cb in @afterAlls
      cb()
    return results

runner = new SpecFrameworkRunner()

When = (contextName, handler) ->
  parentCtx = {children: []}
  isChildCtx = runner.contextStack.length > 0
  ctx = new SpecContext contextName
  if isChildCtx
    parentCtx = _.last runner.contextStack
    parentCtx.children.push ctx
  else
    runner.contexts.push ctx
  runner.contextStack.push ctx
  handler()
  runner.contextStack.pop()

BeforeAll = (handler) ->
  runner.beforeAlls.push handler

Before = (handler) ->
  currCtx = _.last runner.contextStack
  currCtx.befores.push handler

It = (specName, handler) ->
  specification = new Specification specName, handler
  currCtx = _.last runner.contextStack
  currCtx.specs.push specification

After = (handler) ->
  currCtx = _.last runner.contextStack
  currCtx.afters.push handler

AfterAll = (handler) ->
  runner.afterAlls.push handler

return {
  When: When
  Before: Before
  It: It
  After: After
  runner: runner
  assert: assert
  verify: assert
}
