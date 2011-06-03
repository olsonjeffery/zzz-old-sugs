sprites = []

sprite = null
$.startup ->
  sprite = new Sprite "circle_asterisk.png"
  sprite.setPos {x:60, y:70}
  sprites.push sprite

$.render (canvas, input) ->
  canvas.draw sprite
  if input.isKeyDown Keys.Up
    puts 'pushed up key!'
  if input.isKeyDown Keys.Down
    puts 'pushed down key!'
  if input.isKeyDown Keys.Left
    puts 'pushed left key!'
  if input.isKeyDown Keys.Right
    puts 'pushed right key!'
