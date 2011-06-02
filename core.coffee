global = this
# scripts passed into the libScripts params in config
scriptsToBeLoaded = []
# callbacks registered via $.startup()
startupCallbacks = []
# callbacks registered via $.render()
renderCallbacks = []

# the global prefix for loading scripts, assets, etc from the
# loaded module.. specified (relative to the cwd) in config.js
# and combined with getcwd() for a full path
moduleDirectoryPrefix = ''

# Sprite class -- Encapsulates a loaded image and it's position on
# the screen
class Sprite
  constructor: (imgPath) ->
    @nativeSprite = __native_newSprite(moduleDirectoryPrefix + imgPath)

  setPos: (pos) ->
    @nativeSprite.__native_setPos @nativeSprite, pos

  getPos: ->
    @nativeSprite.__native_getPos @nativeSprite

global.Sprite = Sprite
# Canvas class -- The "display surface" that a user draws to during the
# render loop (callbacks registered to $.render() ). Instances of this
# class are *only* available during the render loop.
class Canvas
  constructor: (nativeCanvas) ->
    @nativeCanvas = nativeCanvas

  draw: (sprite) ->
    @nativeCanvas.__native_draw @nativeCanvas, sprite.nativeSprite
global.Canvas = Canvas

class Input
  constructor: (nativeInput) ->
    @nativeInput = nativeInput

  isKeyDown: (key) ->
    @nativeInput.__native_isKeyDown @nativeInput, key

# doStartup() -- Called from native code once on startup to run
# all of the registered callbacks to be ran on startup. Graphics
# has been set up, along with environments.
global.doStartup = ->
  _.each startupCallbacks, (cb) -> cb()

# renderSprites() -- Called from native code at the beginning of
# every render loop to allow user code to process game logic and
# set stuff to be drawn. Before being called, the display is Clear()'d,
# and is Display()'d afterward.
global.renderSprites = (nativeCanvas) ->
  canvas = new Canvas(nativeCanvas)
  #input = new Input(sfmlWindow)
  _.each renderCallbacks, (cb) -> cb(canvas)

# load(path) -- load an external javascript file
loadNoPrefix = (path) ->
  if path.match(/\.coffee$/)
    __native_load_coffee global, path
  else
    global.__native_load global, path
  undefined

global.load = (path) ->
  loadNoPrefix(moduleDirectoryPrefix + path)

# global event registrar/util interface
global.$ = {
  # $.config -- a handler that is invoked with an object hash
  # hash from config.js .. consult config.js for docs on config
  # params
  config: (conf) ->
    # Pluck out the moduleDir and build a prefix dir for loading of
    # assets, etc
    cwd = global.__native_getcwd()
    moduleDirectoryPrefix = cwd + '/'+conf.moduleDir
    moduleDirectoryPrefix = moduleDirectoryPrefix.replace('\\','/')
    lastChar = moduleDirectoryPrefix[moduleDirectoryPrefix.length - 1]
    if lastChar != '/'
      moduleDirectoryPrefix += '/'
    puts "moduleDir: #{moduleDirectoryPrefix}"

    # load up scripts
    _.each conf.libScripts, (v) -> scriptsToBeLoaded.push(v)
    moduleScript = if __native_fileExists(moduleDirectoryPrefix+"module.js") then "module.js" else "module.coffee"
    scriptsToBeLoaded.push moduleDirectoryPrefix + moduleScript
    _.each scriptsToBeLoaded, (v) -> 
      puts "loading #{v}..."
      loadNoPrefix(v)

  # $.startup() -- callbacks registered in this function are called
  # after the graphics system is initialized, but before the render
  # loop begins. Maybe do some sprite/graphics pre-loading here (but
  # there's no canvas to draw to at this point)
  startup: (callback) ->
    startupCallbacks.push callback

  # $.render() -- callbacks registered with this function will be called
  # once at the beginning of every render loop. The display is Clear()'d
  # before every render loop and is Display()'d afterwards. Sprite positioning,
  # draw()'ing, and game logic, etc goes in here. Either register once for your
  # whole program or maybe have each sprite being rendered register its own
  # callback (the latter is probably less efficient).
  render: (callback) ->
    renderCallbacks.push callback
}
