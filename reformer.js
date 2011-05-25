var global = this;
(function() {
  sprites = [];

  var util = {
    wrap: function(obj, fnName) {
      obj[fnName] = function() {
        var args = _.map(arguments, function(v) { return v; });
        args.unshift(obj);
        obj['__native_'+fnName].apply(obj, args);
      };
    }
  };

  global.Sprite = function() {};
  global.Sprite.setup = function(sprite) {
    sprite.setPos = function(pos) {
      sprite.__native_setPos(sprite, pos);
    };
    sprite.getPos = function() {
      return sprite.__native_getPos(sprite);
    }
  };

  global.addSprite = function(imgPath, isPlayer) {
    var sprite = __native_newSprite(imgPath);
    Sprite.setup(sprite);
    return sprite;
  };

  var renderCallbacks = []
  global.renderSprites = function() {
    _.each(renderCallbacks, function(cb) { cb(); });
  };
  // global event registrar/util interface
  global.$ = {
    render: function(callback) {
      renderCallbacks.push(callback);
    }
  };
})()
