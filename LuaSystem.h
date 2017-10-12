#pragma once
#include "entityx/entityx.h"
#include "events.h"
#include "components.h"
#include <map>

#include "LuaSharedPointerConverter.hpp"

extern "C" {
    #include <lua.h>
    #include <lualib.h>
}

#include <luabind/luabind.hpp>
#include "Log.h"

namespace scripting {

    class CLuaSystem : public entityx::System<CLuaSystem>, public entityx::Receiver<CLuaSystem> {
    public:
        CLuaSystem(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events);
        virtual ~CLuaSystem();

        void configure(entityx::ptr<entityx::EventManager> events);
        void update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt);

        void receive(const entityx::EntityCreatedEvent &e);
        void receive(const entityx::EntityDestroyedEvent &e);

        void receive(const KeyEvent &e);
        void receive(const MotionEvent &e);

        void receive(const CreateEntityEvent &e);
        void receive(const MoldEntityEvent &e);

        void receive(const CollisionEvent &e);
        void receive(const ApplyForceEvent &e);
        void receive(const ApplyImpulseEvent &e);

        void receive(const UpdateObserverEvent &e);

        void receive(const QuitEvent &e);

        void receive(const ClientConnectedEvent &e);
        void receive(const ClientDisconnectedEvent &e);
        void receive(const ClientReceiveMessageEvent &e);
        void receive(const ClientSendMessageEvent &e);

        void receive(const PlayerRegisterEvent &e);
        void receive(const PlayerSpawnEvent &e);
        void receive(const PlayerJumpEvent &e);
        void receive(const PlayerMoveEvent &e);
        void receive(const PlayerCastEvent &e);

    private:
        lua_State *create(entityx::Entity entity, entityx::ptr<Script> script);
        void destroy(lua_State *state);

        template <typename E>
        void proxyEvent(const std::string &fname, const E &e) {
            using namespace logging;
            std::string s = "on" + fname;

            for (auto itr : m_states) {
                lua_State *L = itr.second;

                luabind::object f = luabind::globals(L)[s];
                if (f) {
                    try {
                        luabind::call_function<void>(f, e);
                    } catch (const luabind::error &e) {
                        auto script = m_entities->get(itr.first).component<Script>();
                        LOG(ERROR, "Failed to call proxy to " << s << " on script " << script->filename << " " << e.what() << " " << lua_tostring(L, -1));
                    } catch (...) {
                        auto script = m_entities->get(itr.first).component<Script>();
                        LOG(ERROR, "Failed to call proxy to " << s << " on script " << script->filename << " ");
                    }
                }
            }
        }

        void BindCore(lua_State *state);
        void BindComponents(lua_State *state);
        void BindEvents(lua_State *state);

        entityx::ptr<entityx::EntityManager> m_entities;
        entityx::ptr<entityx::EventManager> m_events;

        typedef std::map<entityx::Entity::Id, lua_State *> StateMap;
        StateMap m_states;
    };
}
