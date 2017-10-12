local i = 0

EventManager:emit(CreateEntityEvent("Crate" .. i, "crate", 0.7, 10))
i = i + 1

local player = EntityManager:create()
player:assign(Player("trollkarl"))

EventManager:emit(PlayerRegisterEvent(player, true, true))
