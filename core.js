var global = this;
(function() {
  sprites = [];

  /*
  * Sprite class -- Encapsulates a loaded image and it's position on
  * the screen
  */
  global.Sprite = function(imgPath) {
    this.nativeSprite = __native_newSprite(imgPath);
  };
  global.Sprite.prototype = new Object();
  global.Sprite.prototype.setPos = function(pos) {
    this.nativeSprite.__native_setPos(this.nativeSprite, pos);
  };
  global.Sprite.prototype.getPos = function() {
    return this.nativeSprite.__native_getPos(this.nativeSprite);
  };

  /*
  * Canvas class -- The "display surface" that a user draws to during the
  * render loop (callbacks registered to $.render() ). Instances of this
  * class are *only* available during the render loop.
  */
  global.Canvas = function(nativeCanvas) {
    this.nativeCanvas = nativeCanvas;
  };
  global.Canvas.prototype = new Object();
  global.Canvas.prototype.draw = function(sprite) {
    this.nativeCanvas.__native_draw(this.nativeCanvas, sprite.nativeSprite);
  };

  var scriptsToBeLoaded = [];
  var startupCallbacks = [];
  var renderCallbacks = [];

  /*
  * doStartup() -- Called from native code once on startup to run
  * all of the registered callbacks to be ran on startup. Graphics
  * has been set up, along with environments.
  */
  global.doStartup = function() {
    _.each(startupCallbacks, function(cb) { cb(); });
  };

  /*
  * renderSprites() -- Called from native code at the beginning of
  * every render loop to allow user code to process game logic and
  * set stuff to be drawn. Before being called, the display is Clear()'d,
  * and is Display()'d afterward.
  */
  global.renderSprites = function(nativeCanvas) {
    var canvas = new Canvas(nativeCanvas);
    _.each(renderCallbacks, function(cb) { cb(canvas); });
  };

  /*
  * load(path) -- load an external javascript file
  */
  global.load = function(path) {
    global.__native_load(global, path);
  };

  // global event registrar/util interface
  global.$ = {
    /*
    * Invoked from config.js Does:
    * - Designates the "lib" scripts to be loaded into the JS runtime
    * - Calls out the main game script, which is executed last
    */
    config: function(conf) {
      _.each(conf.lib,function(v) { scriptsToBeLoaded.push(v); });
      scriptsToBeLoaded.push(conf.main);
      _.each(scriptsToBeLoaded, function(v) { global.load(v); });
    },
    /*
    * $.startup() -- callbacks registered in this function are called
    * after the graphics system is initialized, but before the render
    * loop begins. Maybe do some sprite/graphics pre-loading here (but
    * there's no canvas to draw to at this point)
    */
    startup: function(callback) {
      startupCallbacks.push(callback);
    },
    /*
    * $.render() -- callbacks registered with this function will be called
    * once at the beginning of every render loop. The display is Clear()'d
    * before every render loop and is Display()'d afterwards. Sprite positioning,
    * draw()'ing, and game logic, etc goes in here. Either register once for your
    * whole program or maybe have each sprite being rendered register its own
    * callback (the latter is probably less efficient).
    */
    render: function(callback) {
      renderCallbacks.push(callback);
    }
  };
})()
