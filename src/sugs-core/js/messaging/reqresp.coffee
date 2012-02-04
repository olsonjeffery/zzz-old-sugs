###
Copyright 2011 Jeffery Olson <olson.jeffery@gmail.com>. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this list of
    conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice, this list
    of conditions and the following disclaimer in the documentation and/or other materials
    provided with the distribution.

THIS SOFTWARE IS PROVIDED BY JEFFERY OLSON <OLSON.JEFFERY@GMAIL.COM> ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
JEFFERY OLSON <OLSON.JEFFERY@GMAIL.COM> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of Jeffery Olson <olson.jeffery@gmail.com>.
###

class NoRequestHandlerError
  constructor: ->
class DuplicateRequestHandlerError
  constructor: ->
class DuplicateResponseHandlerError
  constructor: ->
class NoTicketForResponseError
  constructor: ->

worker = require 'worker'
pubsub = require 'messaging/pubsub'
global = this

reqRespInit = ->
  @respBindings = {}
  @reqTickets = {}

registerResponseBinding = (msgId, reqHandler) ->
  if not (typeof(@respBindings[msgId]) == 'undefined')
    throw new DuplicateRequestHandlerError()
  @respBindings[msgId] = reqHandler

registerRequestTicket = (ticketId, clientId, msgId, msg, respHandler) ->
  if not (typeof(@respBindings[msgId]) == 'undefined')
    throw new DuplicateResponseHandlerError()
  @reqTickets[ticketId] = respHandler
  wireMsg =
     msg: msg
     clientId: worker.current.getId()
     ticketId: ticketId
     msgId: msgId
  @pubsub.publish clientId, 'sugs:messaging:receive:req', wireMsg

processIncomingRequest = (wireMsg, metaInfo) ->
  {msg, clientId, ticketId, msgId} = wireMsg
  if typeof(@respBindings[msgId]) == 'undefined'
    # error condition
    throw new NoRequestHandlerError()
  else
    handler = @respBindings[msgId]
    pubsubMod = @pubsub
    respObj =             # this needs to have the goods to process
      send: (respMsg) ->  # the response..
        respMsg.ticketId = ticketId
        wireMsgInner =
          msg: respMsg
          ticketId: ticketId
          success: true
        pubsubMod.publish clientId, 'sugs:messaging:receive:resp', wireMsgInner
    try
      handler.call {meta:metaInfo, msg:msg}, msg, respObj, metaInfo
    catch reqHandlerFailError
      errorMsg = ''
      if typeof(reqHandlerFailError.message) != 'undefined'
        errorMsg = reqHandlerFailError.message
      if typeof(e) == 'string'
        errorMsg = reqHandlerFailError
      else

        errorMsg = 'an unknown error has occured in the request handler'
      wireMsg =
        ticketId: ticketId
        msg: errorMsg
        success: false
      pubsubMod.publish clientId, 'sugs:messaging:receive:resp', wireMsg
      throw reqHandlerFailError

processIncomingResponse = (wireMsg, metaInfo) ->
  {msg, ticketId, success} = wireMsg
  metaInfo.requestHandlerSuccess = success
  if typeof(@reqTickets[ticketId]) == 'undefined'
    throw new NoTicketForResponseError()
  else
    handler = @reqTickets[ticketId]
    handler.call {meta: metaInfo, msg: msg}, msg, metaInfo

# doing it this way so that I can return a similar object
# in the module export with pubsubPublish/pubsubSubscribe
# stubbed out for impl by consumers (in reqresp_specs.coffee),
# making this whole module testable
impl =
  init: reqRespInit
  registerResponseBinding: registerResponseBinding
  registerRequestTicket: registerRequestTicket
  processIncomingRequest: processIncomingRequest
  processIncomingResponse: processIncomingResponse
  # these are the actual impls that wire into the
  # pubsub module.. these don't get exported
  pubsub: pubsub
impl.init()

# these are the actual subscriptions to catch stuff
# coming into and out of a module.
pubsub.subscribe 'sugs:messaging:receive:req', (msg) ->
  impl.processIncomingRequest msg, @meta
pubsub.subscribe 'sugs:messaging:receive:resp', (msg) ->
  impl.processIncomingResponse msg, @meta

class eequestResponseMessenger
  constructor: (@pubsub) ->

  req: (clientId, msg, respHandler) ->
    ticket = 'new_ticket'
    reqRespTickets[ticket] =
      clientId: clientId
      msg: msg
      ticket: ticket
      respHandler: respHandler

return {
  # the actual API
  request: (clientId, msgId, msg, responseHandler) ->
     ticketId = "ticket_#{__native_random_uuid()}"
     impl.registerRequestTicket ticketId, clientId, msgId, msg, responseHandler

  respondTo: (msgId, reqHandler) ->
     impl.registerResponseBinding msgId, reqHandler

  # testable doohickey
  testImpl:
    init: reqRespInit
    registerResponseBinding: registerResponseBinding
    registerRequestTicket: registerRequestTicket
    processIncomingRequest: processIncomingRequest
    processIncomingResponse: processIncomingResponse
    # stubs
    publish: ->
  NoRequestHandlerError: NoRequestHandlerError
  DuplicateRequestHandlerError: DuplicateRequestHandlerError
  DuplicateResponseHandlerError: DuplicateResponseHandlerError
  NoTicketForResponseError: NoTicketForResponseError
  }