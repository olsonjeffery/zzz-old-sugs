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

return {
  Specification: Specification
  SpecContext: SpecContext
}