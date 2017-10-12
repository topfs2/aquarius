local animations = require "scripts/animations"

local MAX_SPELLS = 32

local sprites = { }
local i = 1

function onMoldEntityEvent(e)
    local new_sprite

    if (e.mold == "teleport") then
        new_sprite = animations.AnimatedSpriteSheet(e.entity, "textures/heal.png", e.x, e.y, 0, 3, 3, 6, 15)
    elseif (e.mold == "fireball") then
        local w = 1
        local h = 0.8125

        new_sprite = animations.AnimatedSpriteSheet(e.entity, "textures/fireball_wip.png", e.x, e.y, 0, w, h, 8, 15)
        new_sprite.loop = true
    end

    if (new_sprite) then
        if (sprites[i]) then
            sprites[i].destroy()
        end

        sprites[i] = new_sprite

        i = i + 1

        if (i > MAX_SPELLS) then
            i = i - MAX_SPELLS
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
