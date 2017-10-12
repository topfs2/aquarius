#include "PlayerSystem.h"
#include "Log.h"

using namespace logging;

#define PLAYER_MOVE_FORCE

CPlayerSystem::CPlayerSystem(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events)  : m_entities(entities), m_events(events) {
}

void CPlayerSystem::configure(entityx::ptr<entityx::EventManager> events) {
    events->subscribe<PlayerJumpEvent>(*this);
    events->subscribe<PlayerMoveEvent>(*this);
    events->subscribe<CollisionEvent>(*this);
    events->subscribe<KeyEvent>(*this);
}

void CPlayerSystem::receive(const PlayerJumpEvent &e) {
    auto entity = e.entity;
    entityx::ptr<Player> player = entity.component<Player>();

    if (player) {
        m_events->emit<ApplyImpulseEvent>(entity, 0.0f, 5.0f);

        auto sound = m_entities->create();
        sound.assign<Sound>("sound/sfx_jump.wav");
    }
}

void CPlayerSystem::receive(const PlayerMoveEvent &e) {
    auto entity = e.entity;
    entityx::ptr<Player> player = entity.component<Player>();

    if (player) {
        player->direction = e.direction;
        player->state = e.state;
    }
}

void CPlayerSystem::receive(const CollisionEvent &e) {
    auto l = e.left;
    auto r = e.right;
    entityx::ptr<Player> lplayer = l.component<Player>();
    entityx::ptr<Player> rplayer = r.component<Player>();

    // TODO move this
    if (lplayer || rplayer) {
        auto sound = m_entities->create();
        sound.assign<Sound>("sound/jumpland44100.ogg", e.velocity / 50.0f);
    }
}

void CPlayerSystem::receive(const KeyEvent &e) {
}

void CPlayerSystem::update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt) {
    for (auto entity : entities->entities_with_components<Player>()) {
        entityx::ptr<Player> player = entity.component<Player>();
        entityx::ptr<Velocity> velocity = entity.component<Velocity>();

        float dx = velocity ? velocity->x : 0.0f;

        if (player->state.compare("WALKING") == 0) {
            int sign = player->direction.compare("RIGHT") == 0 ? 1 : -1;

#ifdef PLAYER_MOVE_FORCE
            if (sign * dx < 5.0f) {
                float force = sign * 1500.0 * dt;
                events->emit<ApplyForceEvent>(entity, force, 0.0f);
            }
#else
            velocity->x = sign * 5.0;
#endif
        }
    }
}
