local loader = require "scripts/maploader"
local map = require "maps/sunshine"
local spawns = loader(map)

function onPlayerRegisterEvent(e)
    if (e.spawn) then
        local spawn = spawns.player[1] or vec2(0.0, 0.0)
        EventManager:emit(PlayerSpawnEvent(e.entity, spawn.x, spawn.y))
    end
end
