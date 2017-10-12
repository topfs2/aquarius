#pragma once
#include <map>
#include <Box2D/Box2D.h>
#include "entityx/entityx.h"
#include "components.h"
#include "events.h"

namespace physics {
    class CBox2dPhysicsSystem : public entityx::System<CBox2dPhysicsSystem>, public entityx::Receiver<CBox2dPhysicsSystem>, public b2ContactListener
    {
    public:
        CBox2dPhysicsSystem(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events);
        ~CBox2dPhysicsSystem();

        void configure(entityx::ptr<entityx::EventManager> events);

        void receive(const entityx::ComponentRemovedEvent<Body> &e);
        void receive(const entityx::ComponentAddedEvent<Body> &e);
        void receive(const entityx::EntityDestroyedEvent &e);
        void receive(const ApplyImpulseEvent &e);
        void receive(const ApplyForceEvent &e);
        void receive(const SyncEvent &e);

        void update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt);

        virtual void BeginContact(b2Contact* contact);
        virtual void EndContact(b2Contact* contact);
        virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) { }
        virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) { }

    private:
        void CreateRemoveBodies();

        void CreateBody(entityx::Entity::Id id);
        void RemoveBody(entityx::Entity::Id id);

        entityx::ptr<entityx::EntityManager> m_entities;
        entityx::ptr<entityx::EventManager> m_events;

        b2World *m_world;

        typedef std::vector<entityx::ptr<CollisionEvent> > CollisionList;
        CollisionList m_collisions;

        typedef std::vector<entityx::Entity::Id> BodyList;
        BodyList m_create;
        BodyList m_remove;

        typedef std::map<entityx::Entity::Id, b2Body *> BodyMap;
        BodyMap m_bodies;

        bool m_sync;
    };
}
