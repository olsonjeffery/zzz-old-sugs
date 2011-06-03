# kernel.coffee -- Bare functionality to bootstrap without external deps
# (besides underscore and coffeescript). Run seperately and before core.coffee
# so that the latter can declare all our library deps via __loadNoPrefix()
global = this

global.loadNoPrefix = (path) ->
  if path.match(/\.coffee$/)
    __native_load_coffee global, path
  else
    global.__native_load global, path
  undefined

# load(path) -- load an external javascript file
global.load = (path) ->
  loadNoPrefix(global.__modDir + path)
