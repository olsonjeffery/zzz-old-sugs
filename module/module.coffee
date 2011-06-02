showSpritePos = false
sprites = []

sprite = null
$.startup ->
  sprite = new Sprite "circle_asterisk.png"
  sprite.setPos {x:60, y:70}
  sprites.push sprite

$.render (canvas) ->
  if !showSpritePos
    pos = _.first(sprites).getPos()
    x = pos.x
    puts "coffee sprite pos: #{pos.x}, #{pos.y}"
    showSpritePos = true
  canvas.draw sprite
