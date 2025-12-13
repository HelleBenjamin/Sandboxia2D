function on_start()
  print("Hello from mod")
  print("Current game version: " .. get_game_version())
  set_player_pos(10, 10)
  local custom_texture = new_texture("assets/textures/player.png")
  local custom_tile = new_tile(1,1,custom_texture, "Custom tile")
end

function on_tick()
  --print(get_player_pos())
end