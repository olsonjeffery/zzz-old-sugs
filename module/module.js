(function() {
  var addedSprite = false;
  var showSpritePos = false;
  var sprites = [];

  var sprite = null;
  $.startup(function() {
    sprite = new Sprite("circle_asterisk.png");
  });

  $.render(function(canvas) {
    if(!addedSprite) {
      sprites.push(sprite);
      addedSprite = true;
      _.first(sprites).setPos({x:60, y:70});
    }
    if(!showSpritePos) {
      var pos = _.first(sprites).getPos();
      puts('after getPos');
      var x = pos.x;
      puts("sprite pos: "+pos.x+", "+pos.y);
      showSpritePos = true;
    }
    canvas.draw(sprite);
  });
})()
