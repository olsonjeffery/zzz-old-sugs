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

global = this

unresolvedMessages = []
localMsgHandlers = {}
msgExMsgHandlers = {}

pushIntoUnresolved = (msgId, msg) ->
  unresolvedMessages.push {msgId: msgId, msg: msg}

preStartup = true
global.__triggerUnresolvedMessages = ->
  initialKeyCount = _.keys(localMsgHandlers).length
  initialUnresolved = unresolvedMessages
  unresolvedMessages = []
  resolveMessages = (pendingMessages, currentKeyCount) ->
    _.each pendingMessages, (msgInfo) ->
      $.trigger msgInfo.msgId, msgInfo.msg
    newKeyCount =  _.keys(localMsgHandlers).length
    if currentKeyCount == newKeyCount
      if unresolvedMessages.length > 0
        throw "Unable to resolve all pre-startup message $.triggers!"
    else if unresolvedMessages.length > 0
      stillUnresolved = unresolvedMessages
      unresolvedMessages = []
      resolveMessages stillUnresolved, newKeyCount
  resolveMessages initialUnresolved, initialKeyCount
  preStartup = false

global.$.trigger = (msgId, msg) ->
  matched = if typeof localMsgHandlers[msgId] == "undefined" then false else true
  if not matched
    if preStartup
        pushIntoUnresolved msgId, msg
    else
      throw "Unresolved msgId #{msgId} $.trigger'd after startup"
  else
    _.each localMsgHandlers[msgId], (cb) -> cb msg

global.$.bind = (msgId, callback) ->
  if typeof localMsgHandlers[msgId] == "undefined"
    localMsgHandlers[msgId] = []
  localMsgHandlers[msgId].push callback

global.$.subscribe = (msgId, callback) ->
  if typeof msgExMsgHandlers[msgId] == "undefined"
    msgExMsgHandlers[msgId] = []
  msgExMsgHandlers[msgId].push callback
  global.__native_subscribe msgId

global.$.publish = (targetAgentId, msgId, msg) ->
  if typeof msg == "undefined" #called with only two args.. msgId = msg payload
    data =
      msg: msgId
      meta:
        sender: global.sugsConfig.myAgentId
        msgId: targetAgentId
    global.__native_publish_broadcast targetAgentId, JSON.stringify data
  else
    data =
      msg: msg
      meta:
        sender: global.sugsConfig.myAgentId
        msgId: msgId
    global.__native_publish_single_target targetAgentId, msgId, JSON.stringify data

global.__processIncomingMessage = (msgId, jsonData) ->
  if typeof msgExMsgHandlers[msgId] == "undefined"
    throw "No handlers for msg ex registered handler of #{msgId}"
  else
    data = JSON.parse jsonData
    _.each msgExMsgHandlers[msgId], (cb) ->
      cb.call data, data.msg
