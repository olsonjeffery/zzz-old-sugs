{ assert, AssertionError } = require 'spec/assert'

pullMessageFrom = (err) ->
  if typeof(err) == 'string'
    err
  else
    err.message

class Specification
  constructor: (@name, @handler, @results, @cliReporter) ->
    @result = 'undefined'
    @failureMessage = ''

  run: ->
    @results.totalSpecs++
    if typeof(@handler) != 'undefined'
      try
        @handler()
      catch error
        if error instanceof AssertionError
          @failWith error.message
        else
          @errorWith error
      if @result == 'undefined'
        @result = 'success'
        @results.successes++
    else
      @results.unimpl++

  failWith: (message) ->
    @result = 'fail'
    @failureMessage = message
    @results.failures++

  errorWith: (message) ->
    @results.errors++
    @result = 'error'
    @failureMessage = message

  displayResults: ->
    @cliReporter.reportSpecification @name, @result, @failureMessage
    @cliReporter.endSpecification

class SpecContext
  constructor: (@name, @results, @cliReporter) ->
    @befores = []
    @specs = []
    @afters = []
    @children = []
    @result = 'pass'

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

  runAndReportResult: (funcs) ->
    result = true
    message = ''
    for cb in funcs
      try
        cb()
      catch error
        result = false
        message = pullMessageFrom error
    [ result, message ]

  runSpecs: ->
    for spec in @specs
      spec.run()

  errorAllSpecs: (message) ->
    for spec in @specs
      spec.errorWith message

  failWith: (message) ->
    @result = 'fail'
    @results.contextFailures++
    @cliReporter.contextFailure @name, message

  run: ->
    @results.totalContexts++
    [beforesGood, beforeMessage] = @runAndReportResult @befores
    if beforesGood
      @runSpecs()
      @runAndReportResult @afters # going to swallow errors
                                  # in the @afters, for now
      if @hasFailingSpecs
        @result = 'fail'
      @cliReporter.reportContext @name, @result, @hasFailingSpecs()
      @displaySpecResults()
      @cliReporter.endContext()
      for ctx in @children
        ctx.run()
      [aftersGood, afterMsg] = @runAndReportResult @afters
      if not aftersGood
        @failWith afterMsg
        @errorAllSpecs afterMsg
        @cliReporter.endContext()
    else
      @failWith beforeMessage
      @errorAllSpecs beforeMessage
      @cliReporter.endContext()

class SpecFrameworkRunner
  constructor: (@cliReporter) ->
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
    ctx = new SpecContext contextName, @results, @cliReporter
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
    specification = new Specification specName, handler, @results, @cliReporter
    currCtx = @contextStack.last()
    currCtx.specs.push specification

class BaseCliReporter
  constructor: ->
  reportContext: (contextName, result, hasFailingSpecs) ->
  contextFailure: (contextName, message) ->
  reportSpecification: (specName, result, failureMessage) ->
  endSpecification: ->
  endContext: ->

class DefaultCliReporter extends BaseCliReporter
  constructor: ->

  reportContext: (contextName, result, hasFailingSpecs) ->
    failed = ''
    if hasFailingSpecs
      failed = " [FAILED]"
    puts "When #{contextName}#{failed}"

  contextFailure: (contextName, message) ->
    puts "When #{contextName}[FAILED]"
    puts "  ERROR: #{message}"
    puts ""

  reportSpecification: (specName, result, failureMessage) ->
    suffix = ''
    if result == 'fail'
      suffix = ' [FAILED]'
    if result == 'error'
      suffix = ' [ERROR]'
    if result == 'undefined'
      suffix = ' [UNIMPLEMENTED]'
    puts "  It #{specName}#{suffix}"
    if result == 'fail' or result == 'error'
      puts "    ERROR: #{failureMessage}"

  endSpecification: ->

  endContext: ->
     puts ''

runner = new SpecFrameworkRunner(new DefaultCliReporter())

When = (contextName, handler) ->
  runner.addContext contextName, handler

BeforeAll = (handler) ->
  runner.beforeAlls.push handler

Before = (handler) ->
  currCtx = _.last runner.contextStack
  currCtx.befores.push handler

It = (specName, handler) ->
  runner.addSpecification specName, handler

After = (handler) ->
  currCtx = _.last runner.contextStack
  currCtx.afters.push handler

AfterAll = (handler) ->
  runner.afterAlls.push handler

return {
  # our, ahem, "public" exports
  When: When
  It: It
  Before: Before
  After: After
  BeforeAll: BeforeAll
  AfterAll: AfterAll
  assert: assert
  verify: assert

  # These exports are for testing this module
  runner: runner
  SpecContext: SpecContext
  Specification: Specification
  SpecFrameworkRunner: SpecFrameworkRunner
  BaseCliReporter: BaseCliReporter
}
