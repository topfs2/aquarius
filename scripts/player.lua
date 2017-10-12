function distance(x, y)
    return math.sqrt(x * x + y * y)
end

function normalize(x, y)
    d = distance(x, y)

    return x / d, y / d
end

local i = 1
local fireballs = { }

local delete = { } -- TODO remove this when its safe to delete from events

function onCollisionEvent(e)
    for k,v in pairs(fireballs) do
        if (e.left == v or e.right == v) then
            local x = e.x
            local y = e.y

            local other = e.left
            if (k == other) then other = e.right end

            local explosion = EntityManager:create()
            EventManager:emit(MoldEntityEvent(explosion, "explosion", x, y))

            EventManager:emit(ApplyImpulseEvent(other, v.velocity.x, v.velocity.y))
            delete[k] = v
        end
    end
end

function onPlayerSpawnEvent(e)
    print("onPlayerSpawn")

    e.entity:assign(Position(e.x, e.y, 0))
    e.entity:assign(physics.Body(Circle(0.9), 1, 1, true))
    e.entity:assign(Sprite("textures/merlin_walk3.png", 1, 1))
end

function math.sign(x)
  return (x<0 and -1) or 1
end

function onPlayerCastEvent(e)
    local player = e.entity

    if (e.ability == "fireball") then
        local dx, dy = normalize(e.x - player.position.x, e.y - player.position.y)
        local angle = math.deg(math.atan2(dy, dx))

        local fireball = EntityManager:create()
        fireball:assign(physics.Body(Circle(0.2), 10, 1))
        fireball.body.bullet = true
        fireball:assign(Velocity(dx * 20, dy * 20, 0))

        EventManager:emit(MoldEntityEvent(fireball, "fireball", player.position.x + dx, player.position.y + dy))
        fireballs[i] = fireball
        i = i + 1
    elseif (e.ability == "teleport") then
        local old = EntityManager:create()
        EventManager:emit(MoldEntityEvent(old, "teleport", player.position.x, player.position.y))

        player:assign(Position(e.x, e.y, 0))
        player:assign(Velocity(0, 0, 0))

        local new = EntityManager:create()
        EventManager:emit(MoldEntityEvent(new, "teleport", e.x, e.y))
    elseif (e.ability == "bend") then

        local velocity = distance(player.velocity.x, player.velocity.y)
        local input_angle = math.atan2(player.velocity.y, player.velocity.x)
        local output_angle = math.atan2(e.x - player.position.x, e.y - player.position.y)

        vx = math.sin(output_angle) * velocity
        vy = math.cos(output_angle) * velocity
        player.velocity.x = vx
        player.velocity.y = vy
    end
end

function onUpdate(dt)
    for k,v in pairs(delete) do
        if (v:valid()) then v:destroy() end
        fireballs[k] = nil
    end
    delete = { }

    for k,v in pairs(fireballs) do
        if (v:valid()) then
            if (v.velocity and v.position) then
                local angle = math.deg(math.atan2(v.velocity.y, v.velocity.x))
                v.position.angle = angle
            end
        else
            fireballs[k] = nil
        end
    end
end
