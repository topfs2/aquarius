#pragma once
#include "entityx/entityx.h"
#include "events.h"
#include "components.h"

class CMoldSystem : public entityx::System<CMoldSystem>, public entityx::Receiver<CMoldSystem> {
public:
    CMoldSystem(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events) : m_entities(entities), m_events(events) { }
    void configure(entityx::ptr<entityx::EventManager> events);

    void update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt) {}

    void receive(const CreateEntityEvent &e);

private:
    entityx::ptr<entityx::EntityManager> m_entities;
    entityx::ptr<entityx::EventManager> m_events;
};

