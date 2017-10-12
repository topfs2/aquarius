#pragma once
#include <string>
#include <iostream>
#include "entityx/entityx.h"

struct CollisionEvent : public entityx::Event<CollisionEvent> {
    CollisionEvent(entityx::Entity left, entityx::Entity right, float x, float y, float velocity) : left(left), right(right), x(x), y(y), velocity(velocity) {}

    entityx::Entity left, right;
    float x;
    float y;
    float velocity;
};

struct ApplyForceEvent : public entityx::Event<ApplyForceEvent> {
    ApplyForceEvent(entityx::Entity entity, float x, float y) : entity(entity), x(x), y(y) { }

    entityx::Entity entity;
    float x;
    float y;
};

struct ApplyImpulseEvent : public entityx::Event<ApplyImpulseEvent> {
    ApplyImpulseEvent(entityx::Entity entity, float x, float y) : entity(entity), x(x), y(y) { }

    entityx::Entity entity;
    float x;
    float y;
};

struct QuitEvent : public entityx::Event<QuitEvent> {
    QuitEvent(unsigned int code = 0) : code(code) {}

    unsigned int code;
};

struct MotionEvent : public entityx::Event<MotionEvent> {
    MotionEvent(std::string device, std::string key, float x, float y) : device(device), key(key), x(x), y(y) {}

    std::string device;
    std::string key;
    float x;
    float y;
};

struct KeyEvent : public entityx::Event<KeyEvent> {
    KeyEvent(bool state, std::string device, std::string key) : state(state), device(device), key(key) {}

    bool state;
    std::string device;
    std::string key;
};

struct CreateEntityEvent : public entityx::Event<CreateEntityEvent> {
    CreateEntityEvent(std::string name, std::string mold, float x, float y) : name(name), mold(mold), x(x), y(y) {}

    std::string name;
    std::string mold;
    float x;
    float y;
};

struct MoldEntityEvent : public entityx::Event<MoldEntityEvent> {
    MoldEntityEvent(entityx::Entity entity, std::string mold, float x, float y) : entity(entity), mold(mold), x(x), y(y) {}

    entityx::Entity entity;
    std::string mold;
    float x;
    float y;
};

struct CreateLayerEvent : entityx::Event<CreateLayerEvent> {
    CreateLayerEvent(const std::string &name, float x, float y, float z) : name(name), x(x), y(y), z(z) { }

    std::string name;
    float x;
    float y;
    float z;
};

struct UpdateObserverEvent : entityx::Event<UpdateObserverEvent> {
    UpdateObserverEvent(entityx::Entity entity) : entity(entity) { }

    entityx::Entity entity;
};

struct SyncEvent : public entityx::Event<SyncEvent> {
};

struct HostEvent : public entityx::Event<HostEvent> {
    HostEvent(unsigned int port) : port(port) {}

    unsigned int port;
};

struct ConnectEvent : public entityx::Event<ConnectEvent> {
    ConnectEvent(std::string address, unsigned int port) : address(address), port(port) {}

    std::string address;
    unsigned int port;
};

struct DisconnectEvent : public entityx::Event<DisconnectEvent> {
    DisconnectEvent(std::string address, unsigned int port) : address(address), port(port) {}

    std::string address;
    unsigned int port;
};

struct ClientConnectedEvent : public entityx::Event<ClientConnectedEvent> {
    ClientConnectedEvent(std::string address, unsigned int port) : address(address), port(port) {}

    std::string address;
    unsigned int port;
};

struct ClientDisconnectedEvent : public entityx::Event<ClientDisconnectedEvent> {
    ClientDisconnectedEvent(std::string address, unsigned int port) : address(address), port(port) {}

    std::string address;
    unsigned int port;
};

struct ClientReceiveMessageEvent : public entityx::Event<ClientReceiveMessageEvent> {
    ClientReceiveMessageEvent(std::string address, unsigned int port, std::string data) : address(address), port(port), data(data) {}

    std::string address;
    unsigned int port;
    std::string data;
};

struct ClientSendMessageEvent : public entityx::Event<ClientSendMessageEvent> {
    ClientSendMessageEvent(std::string address, unsigned int port, std::string data, bool reliable) : address(address), port(port), data(data), reliable(reliable) {}

    std::string address;
    unsigned int port;
    std::string data;
    bool reliable;
};

struct ClientBroadcastMessageEvent : public entityx::Event<ClientBroadcastMessageEvent> {
    ClientBroadcastMessageEvent(std::string data, bool reliable) : data(data), reliable(reliable) {}

    std::string data;
    bool reliable;
};

struct PlayerRegisterEvent : public entityx::Event<PlayerRegisterEvent> {
    PlayerRegisterEvent(entityx::Entity &entity, bool control, bool spawn) : entity(entity), control(control), spawn(spawn) {}

    entityx::Entity entity;
    bool control;
    bool spawn;
};

struct PlayerSpawnEvent : public entityx::Event<PlayerSpawnEvent> {
    PlayerSpawnEvent(entityx::Entity &entity, float x, float y) : entity(entity), x(x), y(y) {}

    entityx::Entity entity;
    float x;
    float y;
};


struct PlayerJumpEvent : public entityx::Event<PlayerJumpEvent> {
    PlayerJumpEvent(entityx::Entity &entity) : entity(entity) {}

    entityx::Entity entity;
};

struct PlayerMoveEvent : public entityx::Event<PlayerMoveEvent> {
    PlayerMoveEvent(entityx::Entity &entity, std::string direction, std::string state) : entity(entity), direction(direction), state(state) {}

    entityx::Entity entity;
    std::string direction;
    std::string state;
};

struct PlayerCastEvent : public entityx::Event<PlayerCastEvent> {
    PlayerCastEvent(entityx::Entity &entity, std::string ability, float x, float y) : entity(entity), ability(ability), x(x), y(y) {}

    entityx::Entity entity;
    std::string ability;
    float x;
    float y;
};
