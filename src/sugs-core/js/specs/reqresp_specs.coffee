{When, Before, It, After, assert, trap_error} = require 'spec'
reqrespModule = require 'messaging/reqresp'

impl = reqrespModule.testImpl
impl.init()
When "a new request is received with no request handler bound", ->
  error = null
  Before ->
    msg =
      msgId: 'sdfsdfsdf'
      msg:
        payload: 'asd'
      clientId: 'client_1'
      ticketId: '123123123'

    error = trap_error ->
      impl.processIncomingRequest msg

  It "should cause an error", ->
    assert error instanceof reqrespModule.NoRequestHandlerError

When "a new request is received with a request handler bound", ->
  error = null
  msgId = "test:bound_handler"
  responseHandlerCalled = false
  publishWasCalled = false
  magicKey = 'asdasda'
  reqMsgProperlyPassed = false
  Before ->
    impl.publish = (clientId, msgId, msg) ->
      publishWasCalled = true
    msg =
      msgId: msgId
      msg:
        payload: magicKey
      clientId: 'client_1'
      ticketId: '123123123'
    impl.registerResponseBinding msgId, (reqMsg, resp) ->
      responseHandlerCalled = true
      reqMsgProperlyPassed = reqMsg.payload == magicKey
      resp.send
        unique: 'sdsdf2222'
    impl.processIncomingRequest msg

  It "should call the request handler", ->
    assert responseHandlerCalled == true

  It "should publish the response afterwards, when resp.send is called", ->
    assert publishWasCalled == true

  It "should correctly pass the request message across the wire", ->
    assert reqMsgProperlyPassed == true

When "a response is received for an non-existant ticketId", ->
  error = null
  msgId = "test:bound_handler"
  responseHandlerCalled = false
  Before ->
    wireMsg =
      msg:
        payload: {}
      ticketId: '123123123'
    error = trap_error ->
      impl.processIncomingResponse wireMsg

  It "should cause an error", ->
    assert error instanceof reqrespModule.NoTicketForResponseError

When "a response handler is registered for and then a response is received for a valid ticket Id", ->
  error = null
  msgId = "test:bound_handler"
  responseHandlerUnique = null
  publishWasCalled = false
  payload =
    unique: 'ndvkjnldsfg'
  Before ->
    impl.publish = (clientId, msgId, msg) ->
      publishWasCalled = true
    ticketId = '321x'
    clientId = "client_x1"
    respHandler = (respMsg) ->
      responseHandlerUnique = respMsg.unique
    impl.registerRequestTicket ticketId, clientId, 'some:test:msg', {}, respHandler
    wireMsg =
      msg: payload
      ticketId: ticketId
    error = trap_error ->
      impl.processIncomingResponse wireMsg

  It "should not cause an error", ->
    assert error == null

  It "should pass the response message to the response handler", ->
    assert responseHandlerUnique == payload.unique

  It "should attempt to publish the request", ->
    assert publishWasCalled == true

When "an error occurs handling a responder to a request", ->
  error = null
  msgId = "test:req_handler_that_fails"
  responseHandlerCalled = false
  publishWasCalled = false
  failureFlagSet = false
  Before ->
    impl.publish = (clientId, msgId, msg) ->
      publishWasCalled = true
      failureFlagSet = msg.success == false
    msg =
      msgId: msgId
      msg:
        payload: 'asd'
      clientId: 'client_2'
      ticketId: 'zzq123124'
    impl.registerResponseBinding msgId, (reqMsg, resp) ->
      throw "I have failed!"
    error = trap_error ->
      impl.processIncomingRequest msg
  It "should allow the exception to bubble on the worker that throws", ->
    assert error != null
  It "should still publish a response", ->
    assert publishWasCalled == true
  It "should notify the requestor of the failure", ->
    assert failureFlagSet == true

When "attempting to bind multiple responders, within a single worker, to the same msg Id", ->
  error = null
  msgId = "test:bound_multiple_handlers"
  responseHandlerCalled = false
  Before ->
    msg =
      msgId: msgId
      msg:
        payload: 'asd'
      clientId: 'client_1'
      ticketId: '123123123'
    impl.registerResponseBinding msgId, (reqObj, respObj) ->
    error = trap_error ->
      impl.registerResponseBinding msgId, (reqObj, respObj) ->

  It "should cause an error", ->
    assert error instanceof reqrespModule.DuplicateRequestHandlerError

When "executing the req/resp callbacks", ->
  providedReqHandlerThisMeta = false
  providedRespHandlerThisMeta = false
  providedReqHandlerThirdParamMeta = false
  providedRespHandlerSecondParamMeta = false
  msgId = "test:checking_for_meta"
  ticketId = "101010011011"
  clientId = "test_1233311"
  payload = {}
  Before ->
    reqHandler = (reqObj, respObj, meta) ->
      puts "REQ HANDLER THIS.META #{@meta}"
      puts "REQ HANDLER PARAM META #{meta}"
      providedReqHandlerThisMeta = @meta?
      providedReqHandlerThirdParamMeta = meta?
    respHandler = (respMsg, meta) ->
      providedRespHandlerThisMeta = @meta?
      providedRespHandlerSecondParamMeta = meta?
    impl.registerResponseBinding msgId, reqHandler
    impl.registerRequestTicket ticketId, clientId, 'some:test:msg', {}, respHandler
    meta =
      msgId: msgId
      sender: clientId
    wireMsg =
      msg: payload
      ticketId: ticketId
      clientId: clientId
      msgId: msgId
    impl.processIncomingRequest wireMsg, meta
    impl.processIncomingResponse wireMsg, meta

  It "should provide them with a this.meta object in the req handler", ->
    assert providedReqHandlerThisMeta == true

  It "should provide the meta obj as the third param to the reqHandler", ->
    assert providedReqHandlerThirdParamMeta == true

  It "should provide them with a this.meta object in the resp handler", ->
    assert providedRespHandlerThisMeta == true

  It "should provide the meta obj as the second param to the respHandler", ->
    assert providedRespHandlerSecondParamMeta == true