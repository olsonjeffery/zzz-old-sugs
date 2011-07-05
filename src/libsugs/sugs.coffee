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

resolveScriptPath = (path) ->
  result = null
  for p in global.sugsConfig.paths
    fullPath = "#{p}/#{path}"
    if __native_fileExists fullPath
      result = fullPath
  result

# load(path) -- load an external javascript file
global.load = (path) ->
  fullPath = resolveScriptPath path
  if fullPath != null
    if global.pathEndsInDotCoffee fullPath
      __native_load_coffee global, fullPath
    else
      __native_load global, fullPath

# callbacks registered via $.startup()
startupCallbacks = []
# callbacks registered via $.mainLoop()
mainLoopCallbacks = []
# callbacks registered via $.render()
renderCallbacks = []

# doStartup() -- Called from native code once on startup to run
# all of the registered callbacks to be ran on startup. Graphics
# has been set up, along with environments.
global.doStartup = ->
  _.each startupCallbacks, (cb) -> cb()
  global.__triggerUnresolvedMessages()

# renderSprites() -- Called from native code at the beginning of
# every render loop to allow user code to process game logic and
# set stuff to be drawn. Before being called, the display is Clear()'d,
# and is Display()'d afterward.
#
# This is, pretty much, the main hook into the user code's side
# of the main game loop
global.runRender = (nativeCanvas, nativeInput, msElapsed) ->
  canvas = new Canvas(nativeCanvas)
  input = new Input(nativeInput)
  _.each renderCallbacks, (cb) -> cb input, canvas, msElapsed

global.runMainLoop = (msElapsed) ->
  _.each mainLoopCallbacks, (cb) -> cb msElapsed

# global event registrar/util interface
configHasRan = false
sugsConfig = null

global.$ = {
  moduleConfig: (conf) ->
    global.__workers =
      backends: _.map(conf.backends, (b) -> resolveScriptPath b)
      frontend: resolveScriptPath conf.frontend
    puts conf.backends.toString()

  # $.startup() -- callbacks registered in this function are called
  # after the graphics system is initialized, but before the render
  # loop begins. Maybe do some sprite/graphics pre-loading here (but
  # there's no canvas to draw to at this point)
  startup: (callback) ->
    startupCallbacks.push callback

  # $.mainLoop() -- callbacks registered here are called with the input
  # object before the $.render(). Intended for game logic.
  mainLoop: (callback) ->
    mainLoopCallbacks.push callback

  # $.render() -- callbacks registered with this function will be called
  # once at the beginning of every render loop. The display is Clear()'d
  # before every render loop and is Display()'d afterwards. Sprite positioning,
  # draw()'ing, and game logic, etc goes in here. Either register once for your
  # whole program or maybe have each sprite being rendered register its own
  # callback (the latter is probably less efficient).
  render: (callback) ->
    renderCallbacks.push callback
}

entryPointsInContext = []
global.showEntryPoints = ->
  puts "###############"
  puts "ENTRY POINTS IN CONTEXT: #{entryPointsInContext.join(', ')}"
  puts "###############"
global.addEntryPoint = (ep) ->
  entryPointsInContext.push ep

# core js libraries to load
load "types.coffee"
load "drawables.coffee"
load "messaging.coffee"
