global = this

loadNoPrefix("types.coffee")

# scripts passed into the libScripts params in config
scriptsToBeLoaded = []
# callbacks registered via $.startup()
startupCallbacks = []
# callbacks registered via $.mainLoop()
mainLoopCallbacks = []
# callbacks registered via $.render()
renderCallbacks = []

# the global prefix for loading scripts, assets, etc from the
# loaded module.. specified (relative to the cwd) in config.js
# and combined with getcwd() for a full path
global.__modDir = ''

# doStartup() -- Called from native code once on startup to run
# all of the registered callbacks to be ran on startup. Graphics
# has been set up, along with environments.
global.doStartup = ->
  _.each startupCallbacks, (cb) -> cb()

# renderSprites() -- Called from native code at the beginning of
# every render loop to allow user code to process game logic and
# set stuff to be drawn. Before being called, the display is Clear()'d,
# and is Display()'d afterward.
global.renderSprites = (nativeCanvas, nativeInput) ->
  canvas = new Canvas(nativeCanvas)
  input = new Input(nativeInput)
  _.each mainLoopCallbacks, (cb) -> cb input
  _.each renderCallbacks, (cb) -> cb canvas

# global event registrar/util interface
global.$ = {
  # $.config -- a handler that is invoked with an object hash
  # from config.js .. consult config.js for docs on config params
  config: (conf) ->
    # Pluck out the moduleDir and build a prefix dir for loading of
    # assets, etc in our module
    cwd = global.__native_getcwd()
    global.__modDir = cwd + '/'+conf.moduleDir
    global.__modDir = global.__modDir.replace('\\','/')
    lastChar = global.__modDir[global.__modDir.length - 1]
    if lastChar != '/'
      __modDir += '/'
    puts "moduleDir: #{global.__modDir}"

    # load up scripts in the config's libScripts member
    _.each conf.libScripts, (v) -> scriptsToBeLoaded.push(v)

    # find our module script and add it to the list of scripts to be loaded
    moduleScript = if __native_fileExists(global.__modDir+"module.js") then "module.js" else "module.coffee"
    scriptsToBeLoaded.push global.__modDir + moduleScript
    _.each scriptsToBeLoaded, (v) ->
      puts "loading #{v}..."
      loadNoPrefix(v)

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
