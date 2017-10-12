local json = require "scripts/json"

local i = 1
local players = { }
local clients = { }

function createSyncMessage()
    local m = { [ "method" ] = "Sync", [ "players" ] = { } }

    for k,v in pairs(players) do
        if (v:valid()) then
            local p = {}

            p.name = k

            if (v.position) then
                p.x = v.position.x
                p.y = v.position.y
            else
                p.x = 0
                p.y = 0
            end

            if (v.velocity) then
                p.dx = v.velocity.x
                p.dy = v.velocity.y
            else
                p.dx = 0
                p.dy = 0
            end

            m.players[k] = p
        end
    end

    return json.encode(m)
end

function onClientReceiveMessageEvent(e)
--    print("receive", e.data)

    local m = json.decode(e.data)
    if (m.method == "PlayerJump") then
        local e = players[m.name]
        if (e) then
            EventManager:emit(PlayerJumpEvent(e))
            EventManager:emit(ClientBroadcastMessageEvent(json.encode(m), true))
        end
    elseif (m.method == "PlayerMove") then
        local e = players[m.name]
        if (e) then
            EventManager:emit(PlayerMoveEvent(e, m.direction, m.state))
            EventManager:emit(ClientBroadcastMessageEvent(json.encode(m), true))
        end
    elseif (m.method == "PlayerCast") then
        local e = players[m.name]
        if (e) then
            EventManager:emit(PlayerCastEvent(e, m.ability, m.x, m.y))
            EventManager:emit(ClientBroadcastMessageEvent(json.encode(m), true))
        end
    end
end

function onClientSendMessageEvent(e)
--    print("send", e.data)
end

function onClientConnectedEvent(e)
    EventManager:emit(ClientSendMessageEvent(e.address, e.port, createSyncMessage()))

    local name = "player-" .. i
    i = i + 1

    local addr = e.address .. ":" .. e.port
    print("Client connected", addr, name)

    clients[addr] = name

    local player = EntityManager:create()
    player:assign(Player(name))

    players[name] = player

    EventManager:emit(ClientSendMessageEvent(e.address, e.port, '{ "method": "helo", "name": "' .. name ..'" }', true))
    EventManager:emit(PlayerRegisterEvent(player, false, true))
end

function onClientDisconnectedEvent(e)
    local addr = e.address .. ":" .. e.port
    local name = clients[addr]

    print("Client disconnected", addr, name)

    local p = players[name]

    if (p and p:valid()) then
        players[name]:destroy()
        players[name] = nil
    end
end

function onPlayerSpawn(e)
    local m = { ["method"] = "PlayerSpawn" }
    m.name = e.entity.player.name
    m.x = e.entity.position.x
    m.y = e.entity.position.y

    EventManager:emit(ClientBroadcastMessageEvent(json.encode(m), true))
end

local lastPing = 0
local lastSync = 0

function onUpdate(dt)
    lastPing = lastPing + dt
    lastSync = lastSync + dt

    if (lastPing > 1) then
        --EventManager:emit(ClientBroadcastMessageEvent('{ "method": "ping" }'))
        lastPing = 0
    end

    if (lastSync > 0.05) then
        EventManager:emit(ClientBroadcastMessageEvent(createSyncMessage(), false))
        lastSync = 0
    end
end

EventManager:emit(HostEvent(9081));
