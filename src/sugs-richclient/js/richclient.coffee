global = this

types = require 'types'
input = require 'input'

# callbacks registered via $.render()
renderCallbacks = []

# runRunder() -- Called from native code at the beginning of
# every render loop to allow user code to process game logic and
# set stuff to be drawn. Before being called, the display is Clear()'d,
# and is Display()'d afterward.
#
# This is, pretty much, the main hook into the user code's side
# of the main game loop
global.runRender = (nativeCanvas, nativeInput, msElapsed) ->
  canvas = new types.Canvas(nativeCanvas)
  currentInput = new input.CurrentInputState(nativeInput)
  _.each renderCallbacks, (cb) -> cb currentInput, canvas, msElapsed

# $.render() -- callbacks registered with this function will be called
# once at the beginning of every render loop. The display is Clear()'d
# before every render loop and is Display()'d afterwards. Sprite positioning,
# draw()'ing, and game logic, etc goes in here. Either register once for your
# whole program or maybe have each sprite being rendered register its own
# callback (the latter is probably less efficient).
global.$.render = (callback) ->
  renderCallbacks.push callback
