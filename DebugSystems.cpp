#include "DebugSystems.h"
#include "Log.h"

using namespace logging;

void CDebugPhysicsSystem::receive(const CollisionEvent &collision) {
    LOG(DEBUG, "Entities collided: " << collision.left << " and " << collision.right);
}

void DebugInputSystem::receive(const KeyEvent &e) {
    LOG(DEBUG, "Key(" << (e.state ? "true" : "false") << ", " << e.device << ", " << e.key << ")");
}

void DebugInputSystem::receive(const MotionEvent &e) {
    LOG(DEBUG, "Motion(" << e.device << ", " << e.key << ", " << e.x << ", " << e.y << ")");
}

void DebugEntitySystem::receive(const entityx::EntityCreatedEvent &e) {
    LOG(DEBUG, "EntityCreated " << e.entity);
}

void DebugEntitySystem::receive(const entityx::EntityDestroyedEvent &e) {
    LOG(DEBUG, "EntityDestroyed " << e.entity);
}

void DebugNetworkSystem::receive(const ClientConnectedEvent &e) {
    LOG(DEBUG, "ClientConnected " << e.address << ":" << e.port);
}

void DebugNetworkSystem::receive(const ClientDisconnectedEvent &e) {
    LOG(DEBUG, "ClientDisconnected " << e.address << ":" << e.port);
}

void DebugNetworkSystem::receive(const ClientReceiveMessageEvent &e) {
    LOG(DEBUG, "ClientReceiveMessage " << e.address << ":" << e.port << " (" << e.data << ")");
}

void DebugNetworkSystem::receive(const ClientSendMessageEvent &e) {
    LOG(DEBUG, "ClientSendMessage " << e.address << ":" << e.port << " (" << e.data << ")");
}

void CDebugPlayerSystem::receive(const PlayerJumpEvent &e) {
    LOG(DEBUG, "PlayerJumpEvent " << e.entity);
}

void CDebugPlayerSystem::receive(const PlayerMoveEvent &e) {
    LOG(DEBUG, "ClientSendMessage " << e.entity << " Direction::" << e.direction  << " and State::" << e.state);
}
