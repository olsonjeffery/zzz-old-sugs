class AssertionError
  constructor: (@message) ->

assert = (exp, msg = "Assertion failed") ->
  if not exp
    throw new AssertionError msg

return {
  assert: assert
  AssertionError: AssertionError
}