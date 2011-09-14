{When, It} = require 'spec'

When "requiring the spec module" ->
  It "should provide the When function" ->
    typeof(When) == "function"
  It "should provide the It function" ->
    typeof(It) == "function"