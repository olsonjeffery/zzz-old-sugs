sprite = null

$.startup ->
  sprite = new Sprite "circle_asterisk.png"
  sprite.setPos {x:60, y:70}

$.mainLoop (input) ->
  pos = sprite.getPos()
  if input.isKeyDown Keys.Up
    pos = {x: pos.x, y: (if (pos.y - 1) < 0 then 0 else pos.y - 1) }
  if input.isKeyDown Keys.Down
    pos = {x: pos.x, y: pos.y + 1}
  if input.isKeyDown Keys.Left
    pos = {x: (if (pos.x - 1) < 0 then 0 else pos.x - 1), y: pos.y}
  if input.isKeyDown Keys.Right
    pos = {x: pos.x + 1, y: pos.y }
  sprite.setPos pos

$.render (canvas) ->
  canvas.draw sprite
