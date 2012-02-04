{When, Before, It, After, assert, trap_error} = require 'spec'

pubsub = (require 'messaging/pubsub').testImpl

When 'calling publish with three args and the first arg is a string', ->
  error = null
  single_publish_called = false
  pubsub._publishSingle = ->
    single_publish_called = true
  Before ->
    error = trap_error ->
      pubsub.publish 'workerId123', 'some:endpoint'
        msg: ''
  It 'should invoke the single target publish path', ->
    assert single_publish_called = true
  It 'should not cause an error', ->
    assert error == null

When 'calling publish with three args and the first arg is a non-empty array', ->
  error = null
  multi_publish_called = false
  pubsub._publishBroadcastNew = ->
    multi_publish_called = true
  Before ->
    error = trap_error ->
      pubsub.publish ['sdfsd'], 'some:endpoint'
        msg: ''
  It 'should invoke the multiple-target publish path', ->
    assert multi_publish_called == true
  It 'should not cause an error', ->
    assert error == null

When 'calling publish with three args and the first arg is an empty array', ->
  error = null
  Before ->
    error = trap_error ->
      pubsub.publish [], 'some:endpoint'
        msg: ''
  It 'should cause an error', ->
    assert error != null

When 'calling publish less than two arguments', ->
  It 'should cause an error'