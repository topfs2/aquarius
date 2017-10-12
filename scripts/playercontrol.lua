local player
local left = false
local right = false
local mouse = vec2(0, 0)
local camera = EntityManager:create()
camera:assign(Position(0, 0, 0))

EventManager:emit(UpdateObserverEvent(camera))

function onMotionEvent(e)
    mouse.x = e.x * 32 * 0.5
    mouse.y = e.y * -18 * 0.5
end

function onUpdate(dt)
    if (player and player:valid() and player.position) then
        camera:assign(Position(-player.position.x, -player.position.y, 0))
    end
end

function onPlayerRegisterEvent(e)
    if (e.control) then
        player = e.entity
    end
end

function onKeyEvent(e)
    if (player and player:valid() and player.position) then
        local mx = player.position.x + mouse.x
        local my = player.position.y + mouse.y

        if (e.state and e.device == "keyboard" and e.key == "R") then
            EventManager:emit(CreateEntityEvent("Crate" .. i, "crate", mx, my))
            i = i + 1
        elseif (e.state and e.device == "keyboard" and (e.key == "Space" or e.key == "W") and player.body.contacts > 0) then
            EventManager:emit(PlayerJumpEvent(player))
        elseif (e.state and e.device == "keyboard" and e.key == "E") then
            EventManager:emit(PlayerCastEvent(player, "bend", mx, my))
        elseif (e.device == "keyboard" and (e.key == "A" or e.key == "D")) then
            if (e.key == "A") then left = e.state end
            if (e.key == "D") then right = e.state end

            local m = { ["method"] = "PlayerMove", ["name"] = name }
            m.direction = player.player.direction
            m.state = "IDLE"

            if ((e.key == "A" and e.state) or left)  then m.direction = "LEFT" end
            if ((e.key == "D" and e.state) or right) then m.direction = "RIGHT" end
            if (left or right) then m.state = "WALKING" end

            EventManager:emit(PlayerMoveEvent(player, m.direction, m.state))
        elseif (e.state and e.device == "keyboard" and (e.key == "T" or e.key == "F")) then
            local m = { ["method"] = "PlayerCast", ["name"] = name }
            m.x = player.position.x + mouse.x
            m.y = player.position.y + mouse.y
            m.ability = "teleport"

            if (e.key == "F") then m.ability = "fireball" end

            EventManager:emit(PlayerCastEvent(player, m.ability, m.x, m.y))
        end
    end
end
