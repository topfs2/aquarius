local json = require "scripts/json"

local name
local player

local players = { }

function onClientReceiveMessageEvent(e)
--    print("receive", e.data)

    local m = json.decode(e.data)

    if unexpected_condition then 
        print(unexpected_condition)
    end
    if (m.method == "ping") then
        EventManager:emit(ClientSendMessageEvent(e.address, e.port, '{ "method": "pong" }'), true)
    elseif (m.method == "helo") then
        name = m.name
    elseif (m.method == "Sync") then
        for k,v in pairs(m.players) do
            local control = k == name
            local p = players[k]

            if (p == nil) then
                p = EntityManager:create()
                p:assign(Player(k))

                players[k] = p

                if (control) then
                    player = p
                end

                EventManager:emit(PlayerRegisterEvent(p, control, false))
                EventManager:emit(PlayerSpawnEvent(p, v.x, v.y))
            end

            p:assign(Position(v.x,  v.y,  0))
            p:assign(Velocity(v.dx, v.dy, 0))
        end
    elseif (m.method == "PlayerSpawn") then
        local control = m.name == name
        local p = players[m.name]

        if (p == nil) then
            p = EntityManager:create()
            p:assign(Player(m.name))

            players[m.name] = p

            if (control) then
                player = p
            end

            EventManager:emit(PlayerRegisterEvent(player, control, false))
        end

        EventManager:emit(PlayerSpawnEvent(p, m.x, m.y))
    elseif (m.method == "PlayerJump" and m.name ~= name) then
        local e = players[m.name]
        if (e) then
            EventManager:emit(PlayerJumpEvent(e))
        end
    elseif (m.method == "PlayerMove" and m.name ~= name) then
        local e = players[m.name]
        if (e) then
            EventManager:emit(PlayerMoveEvent(e, m.direction, m.state))
        end
    elseif (m.method == "PlayerCast" and m.name ~= name) then
        local e = players[m.name]
        if (e) then
            EventManager:emit(PlayerCastEvent(e, m.ability, m.x, m.y))
        end
    end
end

function onClientSendMessageEvent(e)
--    print("send", e.data)
end

function onClientConnectedEvent(e)
    name = e.address .. e.port

    print("Connected to server", name)
end

function onPlayerJumpEvent(e)
    if (e.entity == player) then
        EventManager:emit(ClientBroadcastMessageEvent('{ "method": "PlayerJump", "name": "' .. name .. '" }', true))
    end
end

function onPlayerMoveEvent(e)
    if (e.entity == player) then
        local m = { ["method"] = "PlayerMove", ["name"] = name }
        m.direction = e.direction
        m.state = e.state

        EventManager:emit(ClientBroadcastMessageEvent(json.encode(m), true))
    end
end

function onPlayerCastEvent(e)
    if (e.entity == player) then
        local m = { ["method"] = "PlayerCast", ["name"] = name }
        m.ability = e.ability
        m.x = e.x
        m.y = e.y

        EventManager:emit(ClientBroadcastMessageEvent(json.encode(m), true))
    end
end

EventManager:emit(ConnectEvent("localhost", 9081));
