When = (contextName, handler) ->
Before = (handler) ->
It = (specName, handler) ->
After = (handler) ->
return {
  When: When
  Before: Before
  It: It
  After: After
}
