#pragma once
#include "entityx/entityx.h"
#include "events.h"
#include "components.h"

class CDebugPhysicsSystem : public entityx::System<CDebugPhysicsSystem>, public entityx::Receiver<CDebugPhysicsSystem> {
public:
  void configure(entityx::ptr<entityx::EventManager> event_manager) {
    event_manager->subscribe<CollisionEvent>(*this);
  }

  void update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt) {}

  void receive(const CollisionEvent &collision);
};

class DebugInputSystem : public entityx::System<DebugInputSystem>, public entityx::Receiver<DebugInputSystem> {
public:
  void configure(entityx::ptr<entityx::EventManager> event_manager) {
    event_manager->subscribe<KeyEvent>(*this);
    event_manager->subscribe<MotionEvent>(*this);
}
  void update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt) {}

  void receive(const KeyEvent &e);
  void receive(const MotionEvent &e);
};

class DebugEntitySystem : public entityx::System<DebugEntitySystem>, public entityx::Receiver<DebugEntitySystem> {
public:
  void configure(entityx::ptr<entityx::EventManager> event_manager) {
    event_manager->subscribe<entityx::EntityCreatedEvent>(*this);
    event_manager->subscribe<entityx::EntityDestroyedEvent>(*this);
}

  void update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt) {}

  void receive(const entityx::EntityCreatedEvent &e);
  void receive(const entityx::EntityDestroyedEvent &e);
};

class DebugNetworkSystem : public entityx::System<DebugNetworkSystem>, public entityx::Receiver<DebugNetworkSystem> {
public:
  void configure(entityx::ptr<entityx::EventManager> event_manager) {
    event_manager->subscribe<ClientConnectedEvent>(*this);
    event_manager->subscribe<ClientDisconnectedEvent>(*this);
    event_manager->subscribe<ClientReceiveMessageEvent>(*this);
    event_manager->subscribe<ClientSendMessageEvent>(*this);
  }

  void update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt) {}

  void receive(const ClientConnectedEvent &e);
  void receive(const ClientDisconnectedEvent &e);
  void receive(const ClientReceiveMessageEvent &e);
  void receive(const ClientSendMessageEvent &e);
};

class CDebugPlayerSystem : public entityx::System<CDebugPlayerSystem>, public entityx::Receiver<CDebugPlayerSystem> {
public:
    CDebugPlayerSystem() {}

    void configure(entityx::ptr<entityx::EventManager> events) {
        events->subscribe<PlayerJumpEvent>(*this);
        events->subscribe<PlayerMoveEvent>(*this);
    }

    void update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt) {}

    void receive(const PlayerJumpEvent &e);
    void receive(const PlayerMoveEvent &e);
};
