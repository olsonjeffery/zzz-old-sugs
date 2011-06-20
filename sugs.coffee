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


global.loadNoPrefix = (path) ->
  if global.pathEndsInDotCoffee path
    __native_load_coffee global, path
  else
    global.__native_load global, path
  undefined

# load(path) -- load an external javascript file
global.load = (path) ->
  loadNoPrefix(global.sugsConfig.moduleDir + path)

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
global.renderSprites = (nativeCanvas, nativeInput, msElapsed) ->
  canvas = new Canvas(nativeCanvas)
  input = new Input(nativeInput)
  _.each mainLoopCallbacks, (cb) -> cb msElapsed
  _.each renderCallbacks, (cb) -> cb input, canvas, msElapsed

# global event registrar/util interface
configHasRan = false
sugsConfig = null

global.$ = {
  # $.config -- a handler that is invoked with an object hash
  # from config.js .. consult config.js for docs on config params
  config: (conf) ->
    if not configHasRan
      configHasRan = true
      # Pluck out the moduleDir and build a prefix dir for loading of
      # assets, etc in our module
      cwd = global.__native_getcwd()
      moduleDir = cwd + '/'+conf.moduleDir
      moduleDir = moduleDir.replace('\\','/')
      lastChar = moduleDir[moduleDir.length - 1]
      if lastChar != '/'
        moduleDir += '/'
      puts "moduleDir: #{moduleDir}"

      # find our module script and add it to the list of scripts to be loaded
      moduleEntryPoint = if __native_fileExists(moduleDir+"module.js") then "module.js" else "module.coffee"

      global.sugsConfig =
        screenWidth: conf.screen.width
        screenHeight: conf.screen.height
        colorDepth: conf.screen.colorDepth
        moduleDir: moduleDir
        moduleEntryPoint: moduleDir+moduleEntryPoint
        entryPointIsCoffee: if (global.pathEndsInDotCoffee moduleEntryPoint) then true else false
    else
      throw "only one call to $.config is allowed to per app"

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

# core js libraries to load
loadNoPrefix "jslib/types.coffee"
loadNoPrefix "jslib/drawables.coffee"
loadNoPrefix "jslib/messaging.coffee"
