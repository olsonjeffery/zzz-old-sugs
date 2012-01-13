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

global.pathEndsInDotCoffee = (path) ->
  path.match(/\.coffee$/)
global.pathEndsInDotJs = (path) ->
  path.match(/\.js$/)

resolveScriptPath = (path) ->
  result = null
  if global.pathEndsInDotCoffee(path) or global.pathEndsInDotJs(path)
    for p in global.sugsConfig.paths
      fullPath = "#{p}/#{path}"
      if __native_fileExists fullPath
        result = fullPath
  else
    jsPath = path + ".js"
    result = resolveScriptPath jsPath
    if result == null
      coffeePath = path + ".coffee"
      result = resolveScriptPath coffeePath
  result

scriptLoadCache = {}
# load(path) -- load an external javascript file
global.fileIsInPath = (path) ->
  result = false
  for p in global.sugsConfig.paths
    fullPath = "#{p}/#{path}"
    if __native_fileExists fullPath
      result = true
      break
  result
global.require = (path) ->
  result = null
  fullPath = resolveScriptPath path
  if typeof scriptLoadCache[fullPath] == "undefined"
    if fullPath != null
      if global.pathEndsInDotCoffee fullPath
        result = __native_require_coffee global, fullPath
      else if global.pathEndsInDotJs fullPath
        result = __native_require global, fullPath
      else
        throw "provided path must end in either .coffee or .js, dude! '#{path}'"
    else
      throw "unable to resolve path '#{path}'!"
    scriptLoadCache[fullPath] = result
  else
    result = scriptLoadCache[fullPath]
  result

# callbacks registered via $.startup()
startupCallbacks = []
# callbacks registered via $.mainLoop()
mainLoopCallback = null

# doStartup() -- Called from native code once on startup to run
# all of the registered callbacks to be ran on startup. Graphics
# has been set up, along with environments.
global.doStartup = ->
  _.each startupCallbacks, (cb) -> cb()

timePassed = 0
lastTickForFps = {}
global.runMainLoop = (msElapsed) ->
  if mainLoopCallback != null
    timePassed += msElapsed
    gap = mainLoopCallback[0]
    lastTime = lastTickForFps[gap]
    sleepTime = (lastTime + gap) - timePassed
    #puts "sleepTime: #{sleepTime} gap: #{gap} lastTime: #{lastTime} timePassed: #{timePassed}"
    if sleepTime > 0
      global.__native_thread_sleep sleepTime
      msElapsed += sleepTime
    lastTickForFps[gap] = timePassed
    cb = mainLoopCallback[1]
    cb msElapsed

global.embedObjectInNamespace = (outter, ns, inner, propName) ->
  all = ns.split '.'
  if ns == ""
    outter[propName] = inner
  else
    tail = (obj, curr, rest) ->
      if typeof(obj[curr]) == "undefined"
        obj[curr] = {}
      obj = obj[curr]
      if rest.length == 0
        obj[propName] = inner
      else
        tail obj, _.first(rest), _.rest(rest)
    tail outter, _.first(all), _.rest(all)
global.embedObjectInNamespace = (outerObj, ns, addedObj) ->
  if ns == ""
    throw "You must provide a valid namespace string to embed an object."
  allNames = ns.split '.'
  if allNames.length == 1
    outerObj[allNames.first()] = addedObj
  else
    namespaceCreator = (currObj, names) ->
      if names.length == 1
        currObj[names.first()] = addedObj
      else
        currName = names.first()
        remainingNames = names.from(1)
        if (typeof currObj[currName]) == "undefined"
          currObj[currName] = {}
        namespaceCreator currObj[currName], remainingNames
    namespaceCreator outerObj, allNames

global.pullObjectFrom = (topLevel, nsPlusPath) ->
  all = ns.split '.'
  if ns == ""
    return topLevel
  else
    tail = (obj, l) ->
      if not obj?
        obj
      if l.length == 1
        obj[_.first(l)]
      else
        tail _.first l, _.rest l
    if not Array.isArray l
      topLevel
    else
      tail topLevel, all

global.$ = {
  moduleConfig: (conf) ->
    global.__workers =
      backends: _.map(conf.backends, (b) -> resolveScriptPath b)
      frontend: resolveScriptPath conf.frontend
    puts conf.backends.toString()

  # $.startup() -- callbacks msElapsed, registered in this function are called
  # after the graphics system is initialized, but before the render
  # loop begins. Maybe do some sprite/graphics pre-loading here (but
  # there's no canvas to draw to at this point)
  startup: (callback) ->
    startupCallbacks.push callback

  # $.mainLoop() -- callbacks registered here are called with the input
  # object before the $.render(). Intended for game logic.
  mainLoop: (fps, callback) ->
    gap = 1000 / fps
    lastTickForFps[gap] = 0
    mainLoopCallback = [ gap, callback ]
}

entryPointsInContext = []
global.showEntryPoints = ->
global.addEntryPoint = (ep) ->
  entryPointsInContext.push ep
