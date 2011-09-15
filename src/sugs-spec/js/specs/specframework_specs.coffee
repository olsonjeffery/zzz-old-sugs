{When, It, assert} = require 'spec' # stuff I actually
                                    # need to write specs

{runner} = require 'spec' # for SUT

When "requiring the spec module", ->
  It "should provide the When function", ->
    assert typeof(When) == "function"
  It "should provide the It function", ->
    assert typeof(It) == "function"
  When "having a nested spec", ->
    topCtx = _.last runner.contextStack
    topLevelContexts = runner.contexts
    putItOnTopSpec = "puts this context on top of the contextStack"

    It "puts this context on top of the context stack", ->
      assert topCtx.name == "having a nested spec"
    It "doesn't add this spec to the list of top-level contexts", ->
      contextsSharingOurName = _.filter topLevelContexts, (c) ->
        c.name == "having a nested spec"
      assert contextsSharingOurName.length == 0

When "having a context that errors during the handler", ->
  It "marks all specs within that context as having failed"

When "having a context that errors during the Before handler", ->
  It "marks all specs within that context as having failed"
  It "marks them as having failed because of the before handlers"

When "having a context that errors during the After handler", ->
  It "marks all specs within that context as having failed"
  It "marks them as having failed because of the after handlers"