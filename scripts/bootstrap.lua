function add_script(s)
    local e = EntityManager:create()
    e:assign(Script(s))
end

add_script("scripts/explosions.lua")
add_script("scripts/spells.lua")
add_script("scripts/player.lua")


function onInit(e)
    if (e.script.argument == "sandbox" or e.script.argument == "client") then
        add_script("scripts/playercontrol.lua")
    end

    add_script("scripts/game.lua")
    add_script("scripts/" .. e.script.argument .. ".lua")
end

