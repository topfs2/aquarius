#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include "entityx/entityx.h"
#include "components.h"
#include "events.h"
#include "systems.h"

using namespace std;

#define SKY 1

#define HAS_GRAPHICS
#define HAS_SOUND

//#define DEBUG_ENTITY
//#define DEBUG_INPUT
//#define DEBUG_PLATFORM
//#define DEBUG_NETWORK
//#define DEBUG_PHYSICS
//#define DEBUG_PLAYER

class Level : public entityx::Manager {
public:
    explicit Level() { }
protected:
    void configure() {
#ifdef HAS_SOUND
        system_manager->add<COpenALSoundSystem>();
#endif

#ifdef HAS_GRAPHICS
        system_manager->add<CSDLPlatformSystem>(this);
        system_manager->add<CSDLOpenGLSystem>();
#endif
        system_manager->add<CMoldSystem>(entity_manager, event_manager);

        system_manager->add<scripting::CLuaSystem>(entity_manager, event_manager);

        system_manager->add<physics::CBox2dPhysicsSystem>(entity_manager, event_manager);

        system_manager->add<CInputSystem>(entity_manager, event_manager);

        system_manager->add<CPlayerSystem>(entity_manager, event_manager);

#ifdef DEBUG_PHYSICS
        system_manager->add<CDebugPhysicsSystem>();
#endif
#ifdef DEBUG_NETWORK
        system_manager->add<DebugNetworkSystem>();
#endif
#ifdef DEBUG_ENTITY
        system_manager->add<DebugEntitySystem>();
#endif
#ifdef DEBUG_INPUT
        system_manager->add<DebugInputSystem>();
#endif
#ifdef DEBUG_PLAYER
        system_manager->add<CDebugPlayerSystem>();
#endif
    };

    void initialize() {
/*
#ifdef DEBUG_ENTITY
        entity_manager->create().assign<Script>("scripts/debug.entity.lua");
#endif
#ifdef DEBUG_INPUT
        entity_manager->create().assign<Script>("scripts/debug.input.lua");
#endif
#ifdef DEBUG_PLATFORM
        entity_manager->create().assign<Script>("scripts/debug.platform.lua");
#endif
#ifdef DEBUG_NETWORK
        entity_manager->create().assign<Script>("scripts/debug.network.lua");
#endif
#ifdef DEBUG_PHYSICS
        entity_manager->create().assign<Script>("scripts/debug.physics.lua");
#endif
#ifdef DEBUG_PLAYER
        entity_manager->create().assign<Script>("scripts/debug.player.lua");
#endif
*/
        auto entry = entity_manager->create();
        entry.assign<Script>("scripts/bootstrap.lua", "sandbox");
    }

    void update(double dt) {
#ifdef HAS_SOUND
        system_manager->update<COpenALSoundSystem>(dt);
#endif
        system_manager->update<CMoldSystem>(dt);

        system_manager->update<CInputSystem>(dt);

        system_manager->update<CPlayerSystem>(dt);
   
        system_manager->update<physics::CBox2dPhysicsSystem>(dt);

        system_manager->update<scripting::CLuaSystem>(dt);

#ifdef DEBUG_PHYSICS
        system_manager->update<CDebugPhysicsSystem>(dt);
#endif
#ifdef DEBUG_NETWORK
        system_manager->update<DebugNetworkSystem>(dt);
#endif
#ifdef DEBUG_ENTITY
        system_manager->update<DebugEntitySystem>(dt);
#endif
#ifdef DEBUG_INPUT
        system_manager->update<DebugInputSystem>(dt);
#endif
#ifdef DEBUG_PLAYER
        system_manager->update<CDebugPlayerSystem>(dt);
#endif

#ifdef HAS_GRAPHICS
        system_manager->update<CSDLOpenGLSystem>(dt);
        system_manager->update<CSDLPlatformSystem>(dt);
#else
        timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 50 * 1000;
        nanosleep(&ts, NULL);
#endif
    }
};

int main (int argc, char *argv[]) {
    std::cout << "Hello World" << std::endl;

    entityx::ptr<Level> level = entityx::ptr<Level>(new Level());
    level->start();

    level->run();

    SDL_Quit();

    std::cout << "Kill world" << std::endl;
    return 0;
}

