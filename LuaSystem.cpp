#include "LuaSystem.h"
#include "Log.h"

#include <luabind/operator.hpp>
#include <luabind/iterator_policy.hpp>

using namespace logging;
using namespace scripting;

template <typename C>
entityx::ptr<C> assign(entityx::Entity entity, entityx::ptr<C> component) {
    return entity.assign<C>(component);
}

template <typename C>
void remove(entityx::Entity entity, entityx::ptr<C> component) {
    entity.remove<C>();
}

void print_entity(entityx::Entity entity) {
    LOG(DEBUG, entity);
}

CLuaSystem::CLuaSystem(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events) : m_entities(entities), m_events(events) {
    LOG(DEBUG, "LuaSystem linked with " << LUA_VERSION);
}

CLuaSystem::~CLuaSystem() {
    for (auto itr : m_states) {
        destroy(itr.second);
    }
    m_states.clear();
}

void CLuaSystem::configure(entityx::ptr<entityx::EventManager> events) {
    events->subscribe<entityx::EntityCreatedEvent>(*this);
    events->subscribe<entityx::EntityDestroyedEvent>(*this);

    events->subscribe<KeyEvent>(*this);
    events->subscribe<MotionEvent>(*this);

    events->subscribe<CreateEntityEvent>(*this);
    events->subscribe<MoldEntityEvent>(*this);

    events->subscribe<CollisionEvent>(*this);
    events->subscribe<ApplyForceEvent>(*this);
    events->subscribe<ApplyImpulseEvent>(*this);

    events->subscribe<UpdateObserverEvent>(*this);

    events->subscribe<QuitEvent>(*this);

    events->subscribe<ClientConnectedEvent>(*this);
    events->subscribe<ClientDisconnectedEvent>(*this);
    events->subscribe<ClientReceiveMessageEvent>(*this);
    events->subscribe<ClientSendMessageEvent>(*this);

    events->subscribe<PlayerRegisterEvent>(*this);
    events->subscribe<PlayerSpawnEvent>(*this);
    events->subscribe<PlayerJumpEvent>(*this);
    events->subscribe<PlayerMoveEvent>(*this);
    events->subscribe<PlayerCastEvent>(*this);
}

void CLuaSystem::update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt) {
    for (auto entity : entities->entities_with_components<Script>()) {
        entityx::ptr<Script> script = entity.component<Script>();
        StateMap::iterator itr = m_states.find(entity.id());

        lua_State *L = NULL;
        if (itr == m_states.end()) {
            L = create(entity, script);
            m_states[entity.id()] = L;
        } else {
            L = itr->second;
        }
    }

    proxyEvent<double>("Update", dt);
}

void CLuaSystem::receive(const entityx::EntityCreatedEvent &e) {
    proxyEvent<entityx::EntityCreatedEvent>("EntityCreatedEvent", e);
}

void CLuaSystem::receive(const entityx::EntityDestroyedEvent &e) {
    proxyEvent<entityx::EntityDestroyedEvent>("EntityDestroyedEvent", e);
}

void CLuaSystem::receive(const KeyEvent &e) {
    proxyEvent<KeyEvent>("KeyEvent", e);
}

void CLuaSystem::receive(const MotionEvent &e) {
    proxyEvent<MotionEvent>("MotionEvent", e);
}

void CLuaSystem::receive(const CreateEntityEvent &e) {
    proxyEvent<CreateEntityEvent>("CreateEntityEvent", e);
}

void CLuaSystem::receive(const MoldEntityEvent &e) {
    proxyEvent<MoldEntityEvent>("MoldEntityEvent", e);
}

void CLuaSystem::receive(const CollisionEvent &e) {
    proxyEvent<CollisionEvent>("CollisionEvent", e);
}

void CLuaSystem::receive(const ApplyForceEvent &e) {
    proxyEvent<ApplyForceEvent>("ApplyForceEvent", e);
}

void CLuaSystem::receive(const ApplyImpulseEvent &e) {
    proxyEvent<ApplyImpulseEvent>("ApplyImpulseEvent", e);
}

void CLuaSystem::receive(const UpdateObserverEvent &e) {
    proxyEvent<UpdateObserverEvent>("UpdateObserverEvent", e);
}

void CLuaSystem::receive(const QuitEvent &e) {
    proxyEvent<QuitEvent>("QuitEvent", e);
}

void CLuaSystem::receive(const ClientConnectedEvent &e) {
    proxyEvent<ClientConnectedEvent>("ClientConnectedEvent", e);
}

void CLuaSystem::receive(const ClientDisconnectedEvent &e) {
    proxyEvent<ClientDisconnectedEvent>("ClientDisconnectedEvent", e);
}

void CLuaSystem::receive(const ClientReceiveMessageEvent &e) {
    proxyEvent<ClientReceiveMessageEvent>("ClientReceiveMessageEvent", e);
}

void CLuaSystem::receive(const ClientSendMessageEvent &e) {
    proxyEvent<ClientSendMessageEvent>("ClientSendMessageEvent", e);
}

void CLuaSystem::receive(const PlayerRegisterEvent &e) {
    proxyEvent<PlayerRegisterEvent>("PlayerRegisterEvent", e);
}

void CLuaSystem::receive(const PlayerSpawnEvent &e) {
    proxyEvent<PlayerSpawnEvent>("PlayerSpawnEvent", e);
}

void CLuaSystem::receive(const PlayerJumpEvent &e) {
    proxyEvent<PlayerJumpEvent>("PlayerJumpEvent", e);
}

void CLuaSystem::receive(const PlayerMoveEvent &e) {
    proxyEvent<PlayerMoveEvent>("PlayerMoveEvent", e);
}

void CLuaSystem::receive(const PlayerCastEvent &e) {
    proxyEvent<PlayerCastEvent>("PlayerCastEvent", e);
}

lua_State *CLuaSystem::create(entityx::Entity entity, entityx::ptr<Script> script) {
    LOG(INFO, "Creating script " << script->filename);

    lua_State *L = luaL_newstate ();
	if (L) {
        luabind::open(L);
        luaL_openlibs(L);

        BindCore(L);
        BindComponents(L);
        BindEvents(L);

        luabind::module(L) [
            luabind::class_<entityx::Entity, entityx::ptr<entityx::Entity> >("Entity")
                .def(luabind::self == entityx::Entity())

// TODO The assign ones does not work, not sure why
                .def("assign", (entityx::ptr<Sprite>                (entityx::Entity::*)(entityx::ptr<Sprite>))             &entityx::Entity::assign<Sprite>)
                .def("assign", (entityx::ptr<Position>              (entityx::Entity::*)(entityx::ptr<Position>))           &entityx::Entity::assign<Position>)
                .def("assign", (entityx::ptr<Velocity>              (entityx::Entity::*)(entityx::ptr<Velocity>))           &entityx::Entity::assign<Velocity>)
                .def("assign", (entityx::ptr<Sound>                 (entityx::Entity::*)(entityx::ptr<Sound>))              &entityx::Entity::assign<Sound>)
                .def("assign", (entityx::ptr<physics::Body>         (entityx::Entity::*)(entityx::ptr<physics::Body>))      &entityx::Entity::assign<physics::Body>)
                .def("assign", (entityx::ptr<Player>                (entityx::Entity::*)(entityx::ptr<Player>))             &entityx::Entity::assign<Player>)
                .def("assign", (entityx::ptr<Layer>                 (entityx::Entity::*)(entityx::ptr<Layer>))              &entityx::Entity::assign<Layer>)
                .def("assign", (entityx::ptr<Script>                (entityx::Entity::*)(entityx::ptr<Script>))             &entityx::Entity::assign<Script>)

                .property("sprite",     &entityx::Entity::component<Sprite>,            (entityx::ptr<Sprite>           (entityx::Entity::*)(entityx::ptr<Sprite>))   &entityx::Entity::assign<Sprite>)
                .property("position",   &entityx::Entity::component<Position>,          (entityx::ptr<Position>         (entityx::Entity::*)(entityx::ptr<Position>)) &entityx::Entity::assign<Position>)
                .property("velocity",   &entityx::Entity::component<Velocity>,          (entityx::ptr<Velocity>         (entityx::Entity::*)(entityx::ptr<Velocity>)) &entityx::Entity::assign<Velocity>)
                .property("sound",      &entityx::Entity::component<Sound>,             (entityx::ptr<Sound>            (entityx::Entity::*)(entityx::ptr<Sound>))    &entityx::Entity::assign<Sound>)
                .property("body",       &entityx::Entity::component<physics::Body>,     (entityx::ptr<physics::Body>    (entityx::Entity::*)(entityx::ptr<physics::Body>))  &entityx::Entity::assign<physics::Body>)
                .property("player",     &entityx::Entity::component<Player>,            (entityx::ptr<Player>           (entityx::Entity::*)(entityx::ptr<Player>))   &entityx::Entity::assign<Player>)
                .property("layer",      &entityx::Entity::component<Layer>,             (entityx::ptr<Layer>            (entityx::Entity::*)(entityx::ptr<Layer>))    &entityx::Entity::assign<Layer>)
                .property("script",     &entityx::Entity::component<Script>,            (entityx::ptr<Script>           (entityx::Entity::*)(entityx::ptr<Script>))   &entityx::Entity::assign<Script>)

                .def("valid", &entityx::Entity::valid)
                .def("id", &entityx::Entity::id)

                .def("destroy", &entityx::Entity::destroy)
        ];

        luabind::module(L) [
            luabind::def("print_entity", print_entity)
        ];

        luabind::module(L) [
            luabind::class_<entityx::EventManager>("EventManager")
                .def("emit", (void (entityx::EventManager::*)(entityx::ptr<ApplyForceEvent>)) &entityx::EventManager::emit<ApplyForceEvent>)
                .def("emit", (void (entityx::EventManager::*)(entityx::ptr<ApplyImpulseEvent>)) &entityx::EventManager::emit<ApplyImpulseEvent>)
                .def("emit", (void (entityx::EventManager::*)(entityx::ptr<CreateLayerEvent>)) &entityx::EventManager::emit<CreateLayerEvent>)
                .def("emit", (void (entityx::EventManager::*)(entityx::ptr<CreateEntityEvent>)) &entityx::EventManager::emit<CreateEntityEvent>)
                .def("emit", (void (entityx::EventManager::*)(entityx::ptr<MoldEntityEvent>)) &entityx::EventManager::emit<MoldEntityEvent>)
                .def("emit", (void (entityx::EventManager::*)(entityx::ptr<UpdateObserverEvent>)) &entityx::EventManager::emit<UpdateObserverEvent>)

                .def("emit", (void (entityx::EventManager::*)(entityx::ptr<HostEvent>)) &entityx::EventManager::emit<HostEvent>)
                .def("emit", (void (entityx::EventManager::*)(entityx::ptr<ConnectEvent>)) &entityx::EventManager::emit<ConnectEvent>)

                .def("emit", (void (entityx::EventManager::*)(entityx::ptr<ClientSendMessageEvent>)) &entityx::EventManager::emit<ClientSendMessageEvent>)
                .def("emit", (void (entityx::EventManager::*)(entityx::ptr<ClientBroadcastMessageEvent>)) &entityx::EventManager::emit<ClientBroadcastMessageEvent>)
                .def("emit", (void (entityx::EventManager::*)(entityx::ptr<ClientReceiveMessageEvent>)) &entityx::EventManager::emit<ClientReceiveMessageEvent>)

                .def("emit", (void (entityx::EventManager::*)(entityx::ptr<PlayerRegisterEvent>)) &entityx::EventManager::emit<PlayerRegisterEvent>)
                .def("emit", (void (entityx::EventManager::*)(entityx::ptr<PlayerSpawnEvent>)) &entityx::EventManager::emit<PlayerSpawnEvent>)
                .def("emit", (void (entityx::EventManager::*)(entityx::ptr<PlayerJumpEvent>)) &entityx::EventManager::emit<PlayerJumpEvent>)
                .def("emit", (void (entityx::EventManager::*)(entityx::ptr<PlayerMoveEvent>)) &entityx::EventManager::emit<PlayerMoveEvent>)
                .def("emit", (void (entityx::EventManager::*)(entityx::ptr<PlayerCastEvent>)) &entityx::EventManager::emit<PlayerCastEvent>)
        ];

        luabind::module(L) [
            luabind::class_<entityx::EntityManager>("EntityManager")
                .def("create", &entityx::EntityManager::create)
        ];

        luabind::globals(L)["EventManager"] = m_events.get();    // Since we keep the shared pointer as member this is safe
        luabind::globals(L)["EntityManager"] = m_entities.get(); // Since we keep the shared pointer as member this is safe

        int res = luaL_dofile(L, "scripts/polyfill.lua");
        if (res != 0) {
            LOG(ERROR, "Failed to introduce polyfill " << lua_tostring(L, -1));
        }

        res = luaL_dofile(L, script->filename.c_str());
        if (res != 0) {
            LOG(ERROR, "Failed to run file " << lua_tostring(L, -1));
        }

        luabind::object init = luabind::globals(L)["onInit"];
        if (init) {
            try {
                luabind::call_function<void>(init, entity);
            } catch (const luabind::error &e) {
                LOG(ERROR, "Failed to call init " << e.what());
            } catch (...) {
                LOG(ERROR, "Failed to call init");
            }
        }
    } else {
        LOG(ERROR, "Failed to create lua context for " << script->filename);
    }

    return L;
}

void CLuaSystem::destroy(lua_State *L) {
    if (L) {
        luabind::object destroy = luabind::globals(L)["destroy"];
        if (destroy) {
            try {
                luabind::call_function<void>(destroy);
            } catch (const luabind::error &e) {
                LOG(ERROR, "Failed to call init " << e.what() << " " << lua_tostring(L, -1));
            } catch (...) {
                LOG(ERROR, "Failed to call init");
            }
        }

        lua_close(L);
    }
}

void CLuaSystem::BindCore(lua_State *L) {
    luabind::module(L) [
        luabind::class_<vec2, entityx::ptr<vec2> >("vec2")
            .def(luabind::constructor<float, float>())
            .def_readwrite("x", &vec2::x)
            .def_readwrite("y", &vec2::y),

        luabind::class_<Shape, entityx::ptr<Shape> >("Shape")
            .def_readonly("type", &Shape::type)
            .def("is", &Shape::is),

        luabind::class_<Circle, Shape, entityx::ptr<Shape> >("Circle")
            .def(luabind::constructor<float>())
            .def_readonly("diameter", &Circle::diameter),

        luabind::class_<Rectangle, Shape, entityx::ptr<Shape> >("Rectangle")
            .def(luabind::constructor<float, float>())
            .def_readonly("width", &Rectangle::width)
            .def_readonly("height", &Rectangle::height),

        luabind::class_<Polygon, Shape, entityx::ptr<Shape> >("Polygon")
            .def(luabind::constructor<>())
            .def_readonly("vertices", &Polygon::vertices, luabind::return_stl_iterator)
            .def("push_back", &Polygon::push_back),

        luabind::class_<Line, Shape, entityx::ptr<Shape> >("Line")
            .def(luabind::constructor<>())
            .def_readonly("vertices", &Line::vertices, luabind::return_stl_iterator)
            .def("push_back", &Line::push_back)
    ];
}

void CLuaSystem::BindComponents(lua_State *L) {
    luabind::module(L) [
        luabind::class_<Sprite, entityx::ptr<Sprite> >("Sprite")
            .def(luabind::constructor<const std::string &, float, float>())
            .def(luabind::constructor<const std::string &, float, float, float, float, float, float>())
            .def_readwrite("filename", &Sprite::filename)
            .def_readwrite("width", &Sprite::width)
            .def_readwrite("height", &Sprite::height)

            .def_readwrite("u0", &Sprite::u0)
            .def_readwrite("v0", &Sprite::v0)
            .def_readwrite("u1", &Sprite::u1)
            .def_readwrite("v1", &Sprite::v1)
    ];

    luabind::module(L) [
        luabind::class_<Position, entityx::ptr<Position> >("Position")
            .def(luabind::constructor<float, float, float>())
            .def_readwrite("x", &Position::x)
            .def_readwrite("y", &Position::y)
            .def_readwrite("angle", &Position::angle)
    ];

    luabind::module(L) [
        luabind::class_<Velocity, entityx::ptr<Velocity> >("Velocity")
            .def(luabind::constructor<float, float, float>())
            .def_readwrite("x", &Velocity::x)
            .def_readwrite("y", &Velocity::y)
            .def_readwrite("angle", &Velocity::angle)
    ];

    luabind::module(L) [
        luabind::class_<Sound, entityx::ptr<Sound> >("Sound")
            .def(luabind::constructor<const std::string &>())
            .def_readonly("filename", &Sound::filename)
            .def_readwrite("volume", &Sound::volume)
            .def_readonly("playing", &Sound::playing)
    ];

    luabind::module(L, "physics") [
        luabind::class_<physics::Body, entityx::ptr<physics::Body> >("Body")
            .def(luabind::constructor<entityx::ptr<Shape>, float, float>())
            .def(luabind::constructor<entityx::ptr<Shape>, float, float, bool>())
            .def(luabind::constructor<entityx::ptr<Shape>, float, float, bool, bool>())
            .def_readonly("shape", &physics::Body::shape)
            .def_readonly("density", &physics::Body::density)
            .def_readonly("friction", &physics::Body::friction)
            .def_readonly("fixed_rotation", &physics::Body::fixed_rotation)
            .def_readonly("sleeping", &physics::Body::sleeping)
            .def_readonly("contacts", &physics::Body::contacts)
    ];

    luabind::module(L) [
        luabind::class_<Player, entityx::ptr<Player> >("Player")
            .def(luabind::constructor<const std::string &>())
            .def_readwrite("name", &Player::name)
            .def_readwrite("direction", &Player::direction)
            .def_readwrite("state", &Player::state)
    ];

    luabind::module(L) [
        luabind::class_<Layer, entityx::ptr<Layer> >("Layer")
            .def(luabind::constructor<const std::string &>())
            .def_readwrite("name", &Layer::name)
    ];

    luabind::module(L) [
        luabind::class_<Script, entityx::ptr<Script> >("Script")
            .def(luabind::constructor<const std::string &>())
            .def_readonly("filename", &Script::filename)
            .def_readonly("argument", &Script::argument)
    ];

    luabind::module(L) [
/*
        luabind::def("assign", assign<Sprite>),
        luabind::def("assign", assign<Position>),
        luabind::def("assign", assign<Velocity>),
        luabind::def("assign", assign<Sound>),
        luabind::def("assign", assign<physics::Body>),
        luabind::def("assign", assign<Player>),
        luabind::def("assign", assign<Layer>),
        luabind::def("assign", assign<Script>),
*/

        luabind::def("remove", remove<Sprite>),
        luabind::def("remove", remove<Position>),
        luabind::def("remove", remove<Velocity>),
        luabind::def("remove", remove<Sound>),
        luabind::def("remove", remove<physics::Body>),
        luabind::def("remove", remove<Player>),
        luabind::def("remove", remove<Layer>),
        luabind::def("remove", remove<Script>)
    ];
}

void CLuaSystem::BindEvents(lua_State *L) {
    luabind::module(L) [
        luabind::class_<CollisionEvent, entityx::ptr<CollisionEvent> >("CollisionEvent")
            .def(luabind::constructor<entityx::Entity, entityx::Entity, float, float, float>())
            .def_readonly("left", &CollisionEvent::left)
            .def_readonly("right", &CollisionEvent::right)
            .def_readonly("x", &CollisionEvent::x)
            .def_readonly("y", &CollisionEvent::y)
            .def_readonly("velocity", &CollisionEvent::velocity)
    ];

    luabind::module(L) [
        luabind::class_<ApplyForceEvent, entityx::ptr<ApplyForceEvent> >("ApplyForceEvent")
            .def(luabind::constructor<entityx::Entity, float, float>())
            .def_readonly("entity", &ApplyForceEvent::entity)
            .def_readonly("x", &ApplyForceEvent::x)
            .def_readonly("y", &ApplyForceEvent::y)
    ];

    luabind::module(L) [
        luabind::class_<ApplyImpulseEvent, entityx::ptr<ApplyImpulseEvent> >("ApplyImpulseEvent")
            .def(luabind::constructor<entityx::Entity, float, float>())
            .def_readonly("entity", &ApplyImpulseEvent::entity)
            .def_readonly("x", &ApplyImpulseEvent::x)
            .def_readonly("y", &ApplyImpulseEvent::y)
    ];

    luabind::module(L) [
        luabind::class_<QuitEvent, entityx::ptr<QuitEvent> >("QuitEvent")
            .def(luabind::constructor<unsigned int>())
            .def_readonly("code", &QuitEvent::code)
    ];

    luabind::module(L) [
        luabind::class_<MotionEvent, entityx::ptr<MotionEvent> >("MotionEvent")
            .def(luabind::constructor<std::string, std::string, float, float>())
            .def_readonly("device", &MotionEvent::device)
            .def_readonly("key", &MotionEvent::key)
            .def_readonly("x", &MotionEvent::x)
            .def_readonly("y", &MotionEvent::y)
    ];

    luabind::module(L) [
        luabind::class_<KeyEvent, entityx::ptr<KeyEvent> >("KeyEvent")
            .def(luabind::constructor<bool, std::string, std::string>())
            .def_readonly("state", &KeyEvent::state)
            .def_readonly("device", &KeyEvent::device)
            .def_readonly("key", &KeyEvent::key)
    ];

    luabind::module(L) [
        luabind::class_<CreateEntityEvent, entityx::ptr<CreateEntityEvent> >("CreateEntityEvent")
            .def(luabind::constructor<std::string, std::string, float, float>())
            .def_readonly("name", &CreateEntityEvent::name)
            .def_readonly("mold", &CreateEntityEvent::mold)
            .def_readonly("x", &CreateEntityEvent::x)
            .def_readonly("y", &CreateEntityEvent::y)
    ];

    luabind::module(L) [
        luabind::class_<MoldEntityEvent, entityx::ptr<MoldEntityEvent> >("MoldEntityEvent")
            .def(luabind::constructor<entityx::Entity, std::string, float, float>())
            .def_readonly("entity", &MoldEntityEvent::entity)
            .def_readonly("mold", &MoldEntityEvent::mold)
            .def_readonly("x", &MoldEntityEvent::x)
            .def_readonly("y", &MoldEntityEvent::y)
    ];

    luabind::module(L) [
        luabind::class_<UpdateObserverEvent, entityx::ptr<UpdateObserverEvent> >("UpdateObserverEvent")
            .def(luabind::constructor<entityx::Entity>())
    ];

    luabind::module(L) [
        luabind::class_<CreateLayerEvent, entityx::ptr<CreateLayerEvent> >("CreateLayerEvent")
            .def(luabind::constructor<std::string, float, float, float>())
            .def_readonly("name", &CreateLayerEvent::name)
            .def_readonly("x", &CreateLayerEvent::x)
            .def_readonly("y", &CreateLayerEvent::y)
            .def_readonly("z", &CreateLayerEvent::z)
    ];

    luabind::module(L) [
        luabind::class_<HostEvent, entityx::ptr<HostEvent> >("HostEvent")
            .def(luabind::constructor<unsigned int>())
            .def_readonly("port", &HostEvent::port)
    ];

    luabind::module(L) [
        luabind::class_<ConnectEvent, entityx::ptr<ConnectEvent> >("ConnectEvent")
            .def(luabind::constructor<std::string, unsigned int>())
            .def_readonly("address", &ConnectEvent::address)
            .def_readonly("port", &ConnectEvent::port)
    ];

    luabind::module(L) [
        luabind::class_<DisconnectEvent, entityx::ptr<DisconnectEvent> >("DisconnectEvent")
            .def(luabind::constructor<std::string, unsigned int>())
            .def_readonly("address", &DisconnectEvent::address)
            .def_readonly("port", &DisconnectEvent::port)
    ];

    luabind::module(L) [
        luabind::class_<ClientConnectedEvent, entityx::ptr<ClientConnectedEvent> >("ClientConnectedEvent")
            .def(luabind::constructor<std::string, unsigned int>())
            .def_readonly("address", &ClientConnectedEvent::address)
            .def_readonly("port", &ClientConnectedEvent::port)
    ];

    luabind::module(L) [
        luabind::class_<ClientDisconnectedEvent, entityx::ptr<ClientDisconnectedEvent> >("ClientDisconnectedEvent")
            .def(luabind::constructor<std::string, unsigned int>())
            .def_readonly("address", &ClientDisconnectedEvent::address)
            .def_readonly("port", &ClientDisconnectedEvent::port)
    ];

    luabind::module(L) [
        luabind::class_<ClientReceiveMessageEvent, entityx::ptr<ClientReceiveMessageEvent> >("ClientReceiveMessageEvent")
            .def(luabind::constructor<std::string, unsigned int, std::string>())
            .def_readonly("address", &ClientReceiveMessageEvent::address)
            .def_readonly("port", &ClientReceiveMessageEvent::port)
            .def_readonly("data", &ClientReceiveMessageEvent::data)
    ];

    luabind::module(L) [
        luabind::class_<ClientSendMessageEvent, entityx::ptr<ClientSendMessageEvent> >("ClientSendMessageEvent")
            .def(luabind::constructor<std::string, unsigned int, std::string, bool>())
            .def_readonly("address", &ClientSendMessageEvent::address)
            .def_readonly("port", &ClientSendMessageEvent::port)
            .def_readonly("data", &ClientSendMessageEvent::data)
            .def_readonly("reliable", &ClientSendMessageEvent::reliable)
    ];

    luabind::module(L) [
        luabind::class_<ClientBroadcastMessageEvent, entityx::ptr<ClientBroadcastMessageEvent> >("ClientBroadcastMessageEvent")
            .def(luabind::constructor<std::string, bool>())
            .def_readonly("data", &ClientBroadcastMessageEvent::data)
            .def_readonly("reliable", &ClientBroadcastMessageEvent::reliable)
    ];

    luabind::module(L) [
        luabind::class_<PlayerRegisterEvent, entityx::ptr<PlayerRegisterEvent> >("PlayerRegisterEvent")
            .def(luabind::constructor<entityx::Entity, bool, bool>())
            .def_readonly("entity", &PlayerRegisterEvent::entity)
            .def_readonly("control", &PlayerRegisterEvent::control)
            .def_readonly("spawn", &PlayerRegisterEvent::spawn)
    ];

    luabind::module(L) [
        luabind::class_<PlayerSpawnEvent, entityx::ptr<PlayerSpawnEvent> >("PlayerSpawnEvent")
            .def(luabind::constructor<entityx::Entity, float, float>())
            .def_readonly("entity", &PlayerSpawnEvent::entity)
            .def_readonly("x", &PlayerSpawnEvent::x)
            .def_readonly("y", &PlayerSpawnEvent::y)
    ];

    luabind::module(L) [
        luabind::class_<PlayerJumpEvent, entityx::ptr<PlayerJumpEvent> >("PlayerJumpEvent")
            .def(luabind::constructor<entityx::Entity>())
            .def_readonly("entity", &PlayerJumpEvent::entity)
    ];

    luabind::module(L) [
        luabind::class_<PlayerMoveEvent, entityx::ptr<PlayerMoveEvent> >("PlayerMoveEvent")
            .def(luabind::constructor<entityx::Entity&, std::string, std::string>())
            .def_readonly("entity", &PlayerMoveEvent::entity)
            .def_readonly("direction", &PlayerMoveEvent::direction)
            .def_readonly("state", &PlayerMoveEvent::state)
    ];

    luabind::module(L) [
        luabind::class_<PlayerCastEvent, entityx::ptr<PlayerCastEvent> >("PlayerCastEvent")
            .def(luabind::constructor<entityx::Entity, std::string, float, float>())
            .def_readonly("entity", &PlayerCastEvent::entity)
            .def_readonly("ability", &PlayerCastEvent::ability)
            .def_readonly("x", &PlayerCastEvent::x)
            .def_readonly("y", &PlayerCastEvent::y)
    ];
}
