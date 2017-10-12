#include "Box2dPhysicsSystem.h"
#include "Log.h"
#include "polypartition.h"

using namespace logging;
using namespace physics;

#define RAD2DEG (180.0f / 3.14f)
#define DEG2RAD (3.14f / 180.0f)

#define DELTA_DISTANCE_UPDATE (0.001)
#define DELTA_ANGLE_UPDATE (0.1)

//#define BOX2D_NEW

CBox2dPhysicsSystem::CBox2dPhysicsSystem(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events) : m_entities(entities), m_events(events) {
    b2Vec2 gravity(0.0f, -10.0f);

    m_world = new b2World(gravity);
	m_world->SetContactListener(this);

    m_sync = false;
}

CBox2dPhysicsSystem::~CBox2dPhysicsSystem() {
}

void CBox2dPhysicsSystem::configure(entityx::ptr<entityx::EventManager> events) {
    events->subscribe<entityx::ComponentRemovedEvent<Body> >(*this);
    events->subscribe<entityx::ComponentAddedEvent<Body> >(*this);
    events->subscribe<entityx::EntityDestroyedEvent>(*this);
    events->subscribe<ApplyImpulseEvent>(*this);
    events->subscribe<ApplyForceEvent>(*this);
    events->subscribe<SyncEvent>(*this);
}

void CBox2dPhysicsSystem::receive(const entityx::EntityDestroyedEvent &e) {
    m_remove.push_back(e.entity.id());
//    RemoveBody(e.entity.id());
}

void CBox2dPhysicsSystem::receive(const entityx::ComponentAddedEvent<Body> &e) {
    m_create.push_back(e.entity.id());
//    CreateBody(e.entity.id());
}

void CBox2dPhysicsSystem::receive(const entityx::ComponentRemovedEvent<Body> &e) {
    m_remove.push_back(e.entity.id());
//    RemoveBody(e.entity.id());
}

void CBox2dPhysicsSystem::receive(const ApplyImpulseEvent &e) {
    BodyMap::iterator itr = m_bodies.find(e.entity.id());
    if (itr != m_bodies.end()) {
        b2Body *body = itr->second;

        body->ApplyLinearImpulse(b2Vec2(e.x, e.y), body->GetWorldCenter(), true);

        // Box2D creates impulses by setting proper velocity, so we need to
        // update our velocity right away so we don't sync it back later in update
        auto entity = m_entities->get(e.entity.id());
        entityx::ptr<Velocity> velocity = entity.component<Velocity>();
        if (velocity) {
            b2Vec2 v = body->GetLinearVelocity();
            float va = body->GetAngularVelocity();

            velocity->x = v.x;
            velocity->y = v.y;
            velocity->angle = va;
        }
    }
}

void CBox2dPhysicsSystem::receive(const ApplyForceEvent &e) {
    BodyMap::iterator itr = m_bodies.find(e.entity.id());
    if (itr != m_bodies.end()) {
        b2Body *body = itr->second;

        body->ApplyForce(b2Vec2(e.x, e.y), body->GetWorldCenter(), true);
    }
}

void CBox2dPhysicsSystem::receive(const SyncEvent &e) {
    m_sync = true;
}

void CBox2dPhysicsSystem::update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt) {
    CreateRemoveBodies();

    for (BodyMap::iterator itr = m_bodies.begin(); itr != m_bodies.end(); itr++) {
        auto entity = m_entities->get(itr->first);

        entityx::ptr<Position> position = entity.component<Position>();
        entityx::ptr<Velocity> velocity = entity.component<Velocity>();

        b2Body *body = itr->second;

        if (position) {
            b2Vec2 bPosition = body->GetPosition();
            float32 bAngle = RAD2DEG * body->GetAngle();

            if (std::abs(position->x - bPosition.x) > DELTA_DISTANCE_UPDATE || std::abs(position->y - bPosition.y) > DELTA_DISTANCE_UPDATE || std::abs(position->angle - bAngle) > DELTA_ANGLE_UPDATE)
            {
//                LOG(DEBUG, "Well, position seems to have changed");
                b2Vec2 p(position->x, position->y);
                body->SetTransform(p, DEG2RAD * position->angle);
                body->SetAwake(true);
            }
        }

        if (velocity) {
    		b2Vec2 bVelocity = body->GetLinearVelocity();
            float32 bAngleVelocity = body->GetAngularVelocity();

            if (std::abs(velocity->x - bVelocity.x) > DELTA_DISTANCE_UPDATE || std::abs(velocity->y - bVelocity.y) > DELTA_DISTANCE_UPDATE || std::abs(velocity->angle - bAngleVelocity) > DELTA_ANGLE_UPDATE)
            {
//                LOG(DEBUG, "Well, velocity seems to have changed" << body << " . " << velocity->x << ", " << velocity->y << " . " << bVelocity.x << ", " << bVelocity.y);
                b2Vec2 v(velocity->x, velocity->y);
                body->SetLinearVelocity(v);
                body->SetAngularVelocity(velocity->angle);
                body->SetAwake(true);
            }
        }
/*
        if (dynamic->sleeping) {
            body->PutToSleep();
        }
*/
    }

    float32 timeStep = dt;
    int32 velocityIterations = 8;
    int32 positionIterations = 3;
    m_world->Step(timeStep, velocityIterations, positionIterations);

    for (CollisionList::iterator itr = m_collisions.begin(); itr != m_collisions.end(); itr++) {
        m_events->emit(*itr);
    }
    m_collisions.clear();

    CreateRemoveBodies();

    for (BodyMap::iterator itr = m_bodies.begin(); itr != m_bodies.end(); itr++) {
        auto entity = m_entities->get(itr->first);

        entityx::ptr<Position> position = entity.component<Position>();
        entityx::ptr<Velocity> velocity = entity.component<Velocity>();
        entityx::ptr<Body> dynamic = entity.component<Body>();

        b2Body *body = itr->second;

        b2Vec2 bPosition = body->GetPosition();
        float32 bAngle = RAD2DEG * body->GetAngle();

        position->x = bPosition.x;
        position->y = bPosition.y;
        position->angle = bAngle;

		b2Vec2 bVelocity = body->GetLinearVelocity();
        float32 bAngleVelocity = body->GetAngularVelocity();
        entity.assign<Velocity>(bVelocity.x, bVelocity.y, bAngleVelocity);

        unsigned int contacts = 0;

        for (b2ContactEdge* ce = body->GetContactList(); ce; ce = ce->next) {
            contacts++;
        }

        dynamic->contacts = contacts;

//        dynamic->sleeping = body->IsSleeping();
    }
}

void CBox2dPhysicsSystem::BeginContact(b2Contact* contact) {
    b2Body *bodyA = contact->GetFixtureA()->GetBody();
    b2Body *bodyB = contact->GetFixtureB()->GetBody();

    bool hasA = false;
    bool hasB = false;

    entityx::Entity A;
    entityx::Entity B;

    for (BodyMap::iterator itr = m_bodies.begin(); itr != m_bodies.end() && !(hasA && hasB); itr++) {
        if (itr->second == bodyA) {
            A = m_entities->get(itr->first);
            hasA = true;
        }

        if (itr->second == bodyB) {
            B = m_entities->get(itr->first);
            hasB = true;
        }
    }

    if (hasA && hasB && A.valid() && B.valid()) {
        b2WorldManifold manifold;
        contact->GetWorldManifold(&manifold);

        b2Vec2 p = manifold.points[0];

        float velocity = 1.0;//sqrt(point->velocity.x * point->velocity.x + point->velocity.y * point->velocity.y);
        m_collisions.push_back(entityx::ptr<CollisionEvent>(new CollisionEvent(A, B, p.x, p.y, velocity)));
    } else {
        LOG(WARNING, "Failed to find entities for all contacts");
    }
}

void CBox2dPhysicsSystem::EndContact(b2Contact* contact) {
/*
    b2Body *bodyA = point->shape1->GetBody();
    b2Body *bodyB = point->shape2->GetBody();
*/
}

void CBox2dPhysicsSystem::CreateRemoveBodies() {
    for (BodyList::iterator itr = m_remove.begin(); itr != m_remove.end(); itr++) {
        RemoveBody(*itr);
    }

    for (BodyList::iterator itr = m_create.begin(); itr != m_create.end(); itr++) {
        CreateBody(*itr);
    }

    m_remove.clear();
    m_create.clear();
}

void CBox2dPhysicsSystem::CreateBody(entityx::Entity::Id id) {
    RemoveBody(id);

    auto entity = m_entities->get(id);
    entityx::ptr<Position> position = entity.component<Position>();
    entityx::ptr<Velocity> velocity = entity.component<Velocity>();
    entityx::ptr<Body> dynamic = entity.component<Body>();

    b2BodyDef bodyDef;
    bodyDef.fixedRotation = dynamic->fixed_rotation;
    bodyDef.type = dynamic->density < 0.01 ? b2_staticBody : b2_dynamicBody;
    bodyDef.bullet = dynamic->bullet;

    if (position) {
        bodyDef.position.Set(position->x, position->y);
        bodyDef.angle = DEG2RAD * position->angle;
    }

    if (velocity) {
        bodyDef.linearVelocity.Set(velocity->x, velocity->y);
        bodyDef.angularVelocity = DEG2RAD * velocity->angle;
    }

    b2Body *body = m_world->CreateBody(&bodyDef);

    b2FixtureDef fixtureDef;
    fixtureDef.density = dynamic->density;
    fixtureDef.friction = dynamic->friction;

    if (dynamic->shape->is("rectangle")) {
        entityx::ptr<Rectangle> rectangle = std::dynamic_pointer_cast<Rectangle>(dynamic->shape);

        b2PolygonShape shapeDef;
        shapeDef.SetAsBox(rectangle->width / 2.0, rectangle->height / 2.0);

        fixtureDef.shape = &shapeDef;
        body->CreateFixture(&fixtureDef);
    } else if (dynamic->shape->is("circle")) {
        entityx::ptr<Circle> circle = std::dynamic_pointer_cast<Circle>(dynamic->shape);

        b2CircleShape shapeDef;
        shapeDef.m_radius = circle->diameter * 0.5f;

        fixtureDef.shape = &shapeDef;
        body->CreateFixture(&fixtureDef);
    } else if (dynamic->shape->is("polygon")) {
/*
        entityx::ptr<Polygon> polygon = std::dynamic_pointer_cast<Polygon>(dynamic->shape);

        unsigned int size = polygon->vertices.size();

        if (size >= 3 && size < b2_maxPolygonVertices) {
            LOG(DEBUG, "Polygon " << size);

            b2Vec2 vertices[size];

            for (unsigned int i = 0; i < size; i++) {
                vertices[i] = b2Vec2(polygon->vertices[i].x, polygon->vertices[i].y);
            }

            b2PolygonShape shapeDef;
            shapeDef.Set(vertices, size);

            fixtureDef.shape = &shapeDef;
            body->CreateFixture(&fixtureDef);
        } else {
            LOG(WARNING, "Failed to create polygon due to size " << size);
        }
*/
        entityx::ptr<Polygon> polygonC = std::dynamic_pointer_cast<Polygon>(dynamic->shape);
        unsigned int size = polygonC->vertices.size();
        TPPLPartition pp;

        if (size >= 3) {
            LOG(DEBUG, "Polygon " << size);
            TPPLPoly polygon;
            polygon.Init(size);

            for (unsigned int i = 0; i < size; i++) {
                polygon[i].x = polygonC->vertices[i].x;
                polygon[i].y = polygonC->vertices[i].y;
            }

            polygon.SetOrientation(TPPL_CCW);
            std::list<TPPLPoly> triangles;
            pp.Triangulate_EC(&polygon, &triangles);

            for (std::list<TPPLPoly>::iterator itr = triangles.begin(); itr != triangles.end(); itr++) {
                b2PolygonShape shapeDef;

                itr->SetOrientation(TPPL_CW);

                b2Vec2 vertices[3];
                vertices[0].x = itr->GetPoint(0).x;
                vertices[0].y = itr->GetPoint(0).y;

                vertices[1].x = itr->GetPoint(1).x;
                vertices[1].y = itr->GetPoint(1).y;

                vertices[2].x = itr->GetPoint(2).x;
                vertices[2].y = itr->GetPoint(2).y;

                shapeDef.Set(vertices, 3);

                fixtureDef.shape = &shapeDef;
                body->CreateFixture(&fixtureDef);
            }
        }
    } else if (dynamic->shape->is("line")) {
        entityx::ptr<Line> line = std::dynamic_pointer_cast<Line>(dynamic->shape);

        unsigned int size = line->vertices.size();

        b2Vec2 vertices[size];

        for (unsigned int i = 0; i < size; i++) {
            vertices[i] = b2Vec2(line->vertices[i].x, line->vertices[i].y);
        }

        b2ChainShape shapeDef;
        shapeDef.CreateChain(vertices, size);

        fixtureDef.shape = &shapeDef;
        body->CreateFixture(&fixtureDef);
    }

    m_bodies[entity.id()] = body;
}

void CBox2dPhysicsSystem::RemoveBody(entityx::Entity::Id id) {
    BodyMap::iterator itr = m_bodies.find(id);

    if (itr != m_bodies.end()) {
        m_world->DestroyBody(itr->second);
        m_bodies.erase(itr);
    }
}
