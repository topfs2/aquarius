#pragma once
#include "entityx/entityx.h"
#include "events.h"
#include "components.h"

class CPlayerSystem : public entityx::System<CPlayerSystem>, public entityx::Receiver<CPlayerSystem> {
public:
    CPlayerSystem(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events);

    void configure(entityx::ptr<entityx::EventManager> event_manager);

    void receive(const PlayerJumpEvent &e);
    void receive(const PlayerMoveEvent &e);
    void receive(const CollisionEvent &e);
    void receive(const KeyEvent &e);

    void update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt);

private:
    entityx::ptr<entityx::EntityManager> m_entities;
    entityx::ptr<entityx::EventManager> m_events;
};
