{When, Before, It, After, assert, trap_error} = require 'spec'

When "getting a random uuid id", ->
  result = ''
  Before ->
    result = __native_random_uuid()

  It "should return one", ->
    assert result != ''