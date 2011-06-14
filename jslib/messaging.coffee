global = this

unresolvedMessages = []
msgHandlers = {}

pushIntoUnresolved = (msgId, msg) ->
  unresolvedMessages.push {msgId: msgId, msg: msg}

preStartup = true
global.__triggerUnresolvedMessages = ->
  initialKeyCount = _.keys(msgHandlers).length
  initialUnresolved = unresolvedMessages
  unresolvedMessages = []
  resolveMessages = (pendingMessages, currentKeyCount) ->
    _.each pendingMessages, (msgInfo) ->
      $.trigger msgInfo.msgId, msgInfo.msg
    newKeyCount =  _.keys(msgHandlers).length
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
  matched = if typeof msgHandlers[msgId] == "undefined" then false else true
  if not matched
    if preStartup
        pushIntoUnresolved msgId, msg
    else
      throw "Unresolved msgId #{msgId} $.trigger'd after startup"
  else
      _.each msgHandlers[msgId], (cb) -> cb msg

global.$.bind = (msgId, callback) ->
  if typeof msgHandlers[msgId] == "undefined"
    msgHandlers[msgId] = []
  msgHandlers[msgId].push callback
