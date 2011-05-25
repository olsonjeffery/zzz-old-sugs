(function() {
  var addedSprite = false;
  var showSpritePos = false;

  $.render(function() {
    if(!addedSprite) {
      sprites.push(addSprite("circle_asterisk.png"));
      addedSprite = true;
      _.first(sprites).setPos({x:60, y:70});
      puts('herp');
    }
    if(!showSpritePos) {
      var pos = _.first(sprites).getPos();
      var x = pos.x;
      puts("sprite pos: "+pos.x+", "+pos.y);
      showSpritePos = true;
    }
    //puts("rendering sprites...");
  });
})()
