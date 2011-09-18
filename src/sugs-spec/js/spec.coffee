{ assert, AssertionError } = require 'spec/assert'
{ DefaultCliReporter, BaseCliReporter } = (require 'spec/clireporter')
{ SpecContext, Specification } = require 'spec/framework'
SpecFrameworkRunner = require 'spec/specrunner'

runner = new SpecFrameworkRunner new DefaultCliReporter(), SpecContext, Specification

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
