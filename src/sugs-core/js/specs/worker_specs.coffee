{When, Before, It, After, assert, trap_error} = require 'spec'

worker = require 'worker'

When "examining worker.current", ->
  It "should always be available", ->
    assert typeof(worker.current) != 'undefined'

  It "should have a getPaths() method", ->
    assert worker.current.getPaths().length > 0

  It "should allow getting the workerId", ->
    assert worker.current.getId().startsWith('spec_runner')

  It "should have a kill() func", ->
    assert typeof(worker.current.kill) == 'function'