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

return {
  BaseCliReporter: BaseCliReporter
  DefaultCliReporter: DefaultCliReporter
}