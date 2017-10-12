#include "MoldSystem.h"
#include "Log.h"

using namespace logging;

void CMoldSystem::configure(entityx::ptr<entityx::EventManager> events) {
    events->subscribe<CreateEntityEvent>(*this);
}

void CMoldSystem::receive(const CreateEntityEvent &e) {
    if (e.mold.compare("crate") == 0) {
        auto entity = m_entities->create();
        entity.assign<NameComponent>(e.name);
        entity.assign<Position>(e.x, e.y, 0.0);
        entity.assign<physics::Body>(entityx::ptr<Rectangle>(new Rectangle(1.0f, 1.0f)), 1.0f, 0.3f);
        entity.assign<Sprite>("textures/obj_crate005.png", 1.0f, 1.0f);
    } else if (e.mold.compare("player") == 0) {
        auto entity = m_entities->create();
        entity.assign<Player>(e.name);
        entity.assign<Position>(e.x, e.y, 0.0);
        entity.assign<physics::Body>(entityx::ptr<Circle>(new Circle(0.5f)), 1.0f, 40.0f, true);
        entity.assign<Sprite>("textures/merlin_walk3.png", 1.0f, 1.0f);
    } else {
        auto entity = m_entities->create();
        m_events->emit<MoldEntityEvent>(entity, e.mold, e.x, e.y);
    }
}
