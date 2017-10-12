local animations = require "scripts/animations"

local MAX_EXPLOSIONS = 32

local sprites = { }
local i = 1

function onMoldEntityEvent(e)
    if (e.mold == "explosion") then
        if (sprites[i]) then
            sprites[i].destroy()
        end

        sprites[i] = animations.AnimatedSprite(e.entity, "textures/explosions/expl_09_", 4, "png", e.x, e.y, math.random(360), 3, 3, 32, 32)
        i = i + 1

        if (i > MAX_EXPLOSIONS) then
            i = i - MAX_EXPLOSIONS
        end
    end
end

function onUpdate(dt)
    for k,v in pairs(sprites) do
        if (v.valid()) then
            v.update(dt)
        else
            sprites[k] = nil
        end
    end
end
