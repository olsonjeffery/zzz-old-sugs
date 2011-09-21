# stuff I actually need to write specs
{When, It, Before, After, assert, trap_error} = require 'spec'

specModule = require 'spec' # for SUT

When "requiring the spec module", ->
  It "should provide the When function", ->
    assert typeof(specModule.When) == "function"
  It "should provide the It function", ->
    assert typeof(specModule.It) == "function"
  It "should provide the Before function", ->
    assert typeof(specModule.Before) == "function"
  It "should provide the After function", ->
    assert typeof(specModule.After) == "function"
  It "should provide the BeforeAll function", ->
    assert typeof(specModule.BeforeAll) == "function"
  It "should provide the AfterAll function", ->
    assert typeof(specModule.AfterAll) == "function"
  It "should provide the assert function", ->
    assert typeof(specModule.assert) == "function"
  It "should provide the verify function", ->
    assert typeof(specModule.verify) == "function"

  nestedContextName = "having a nested context"
  When nestedContextName, ->
    topCtx = _.last specModule.runner.contextStack
    topLevelContexts = specModule.runner.contexts

    It "puts this context on top of the context stack", ->
      assert topCtx.name == nestedContextName
    It "doesn't add this spec to the list of top-level contexts", ->
      contextsSharingOurName = _.filter topLevelContexts, (c) ->
        c.name == nestedContextName
      assert contextsSharingOurName.length == 0

When "having a context that errors during the handler", ->
  reporter = new specModule.BaseCliReporter
  runnerUnderTest = new specModule.SpecFrameworkRunner(reporter, specModule.SpecContext, specModule.Specification)
  Before ->
    runnerUnderTest.addContext "errors during handler", ->
      throw "barf!"

  It "should mark the context as failing", ->
    assert runnerUnderTest.contexts.last().result == 'fail'

When "having a context with 2 specs that errors during the Before handler", ->
  shouldBeFalse = false
  reporter = new specModule.BaseCliReporter
  runnerUnderTest = new specModule.SpecFrameworkRunner(reporter, specModule.SpecContext, specModule.Specification)
  stubSpec = ->
  secondStubSpec = ->
    shouldBeFalse = true
  currCtx = null
  errorMessage = "sdfsdfsdf2232451!"

  Before ->
    runnerUnderTest.addContext "error during before handler", ->
      # have to do it in here because of the contextStack..
      # can't use When/It/etc when constructing contexts/specs
      # for testin
      runnerUnderTest.addSpecification "1", stubSpec
      runnerUnderTest.addSpecification "2", secondStubSpec

    currCtx = _.last runnerUnderTest.contexts

    currCtx.befores.push ->
      throw errorMessage

    testResults = currCtx.run()

  It "should mark the context as failed", ->
    assert currCtx.result == 'fail'

  It "marks all specs within that context as having error'd", ->
    failingSpecs = _.select currCtx.specs, (spec) ->
      spec.result == 'error'
    assert failingSpecs.length == 2

  It "marks them as having failed because of the before handlers", ->
    failingSpecs = _.select currCtx.specs, (spec) ->
      spec.failureMessage == errorMessage
    assert failingSpecs.length == 2

  It "should not have allowed the specs to run and modify state", ->
    assert shouldBeFalse == false

When "having a context that errors during the After handler", ->
  shouldBeFalse = false
  reporter = new specModule.BaseCliReporter
  runnerUnderTest = new specModule.SpecFrameworkRunner(reporter, specModule.SpecContext, specModule.Specification)
  stubSpec = ->
  secondStubSpec = ->
    shouldBeFalse = true
  currCtx = null
  errorMessage = "aftersdfsdfsdf2232451!"

  Before ->
    runnerUnderTest.addContext "error during before handler", ->
      # have to do it in here because of the contextStack..
      # can't use When/It/etc when constructing contexts/specs
      # for testin
      runnerUnderTest.addSpecification "1", stubSpec
      runnerUnderTest.addSpecification "2", secondStubSpec

    currCtx = _.last runnerUnderTest.contexts

    currCtx.afters.push ->
      throw errorMessage

    testResults = currCtx.run()

  It "should mark the context as failed", ->
    assert currCtx.result == 'fail'

  It "marks all specs within that context as having error'd", ->
    failingSpecs = _.select currCtx.specs, (spec) ->
      spec.result == 'error'
    assert failingSpecs.length == 2

  It "marks them as having failed because of the after handlers", ->
    failingSpecs = _.select currCtx.specs, (spec) ->
      spec.failureMessage == errorMessage
    assert failingSpecs.length == 2

  It "should have allowed the specs to run and modify state", ->
    assert shouldBeFalse == true

When "using trap_error and no exception bubbles from the input func", ->
  result = 'sdfsdf'
  Before ->
    result = trap_error ->
  It "should return a null value", ->
    assert result == null

class TrapError
  constructor: ->
When "using trap_error and an exception bubbles from the input func", ->
  result = null
  Before ->
    result = trap_error ->
      throw new TrapError()
  It "should return the instance of that error", ->
    assert result instanceof TrapError

When "using trap_error and a string bubbles as an error from the input func", ->
  result = null
  errorMessage = "SDFSDfsdf212342!!!!xxxx"
  Before ->
    result = trap_error ->
      throw errorMessage
  It "should return the string", ->
    assert result == errorMessage
  It "should be an instance of a string", ->
    assert typeof(result) == "string"