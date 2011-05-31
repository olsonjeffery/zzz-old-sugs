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
  * Canvas class -- The "display surface" that a user draws to
  */
  global.Canvas = function(nativeCanvas) {
    this.nativeCanvas = nativeCanvas;
  };
  global.Canvas.prototype = new Object();
  global.Canvas.prototype.draw = function(sprite) {
    this.nativeCanvas.__native_draw(this.nativeCanvas, sprite.nativeSprite);
  };

  var renderCallbacks = []
  global.renderSprites = function(nativeCanvas) {
    var canvas = new Canvas(nativeCanvas);
    _.each(renderCallbacks, function(cb) { cb(canvas); });
  };
  // global event registrar/util interface
  global.$ = {
    render: function(callback) {
      renderCallbacks.push(callback);
    }
  };
})()
