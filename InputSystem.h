#pragma once
#include "entityx/entityx.h"
#include "events.h"
#include "components.h"

class CInputSystem : public entityx::System<CInputSystem>, public entityx::Receiver<CInputSystem> {
public:
    CInputSystem(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events) : m_entities(entities), m_events(events) { }
    void configure(entityx::ptr<entityx::EventManager> events) {
        events->subscribe<KeyEvent>(*this);
    }

    void update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt) {}

    void receive(const KeyEvent &e);

private:
    entityx::ptr<entityx::EntityManager> m_entities;
    entityx::ptr<entityx::EventManager> m_events;
};

