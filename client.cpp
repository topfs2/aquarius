#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include "entityx/entityx.h"
#include "components.h"
#include "events.h"
#include "systems.h"

#define SKY 1
#define HAS_GRAPHICS
#define HAS_SOUND

class Level : public entityx::Manager {
public:
    explicit Level() {}

protected:
    void configure() {
#ifdef HAS_SOUND
        system_manager->add<COpenALSoundSystem>();
#endif

#ifdef HAS_GRAPHICS
        system_manager->add<CSDLPlatformSystem>(this);
        system_manager->add<CSDLOpenGLSystem>();
#endif

        system_manager->add<scripting::CLuaSystem>(entity_manager, event_manager);

        system_manager->add<physics::CBox2dPhysicsSystem>(entity_manager, event_manager);

        system_manager->add<CInputSystem>(entity_manager, event_manager);

        system_manager->add<CPlayerSystem>(entity_manager, event_manager);

        system_manager->add<CENetNetworkSystem>();

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
        auto entry = entity_manager->create();
        entry.assign<Script>("scripts/bootstrap.lua", "client");
    }

    void update(double dt) {
#ifdef HAS_SOUND
        system_manager->update<COpenALSoundSystem>(dt);
#endif

        system_manager->update<CInputSystem>(dt);

        system_manager->update<CPlayerSystem>(dt);
   
        system_manager->update<physics::CBox2dPhysicsSystem>(dt);

        system_manager->update<scripting::CLuaSystem>(dt);

        system_manager->update<CENetNetworkSystem>(dt);

#ifdef HAS_GRAPHICS
        system_manager->update<CSDLOpenGLSystem>(dt);
        system_manager->update<CSDLPlatformSystem>(dt);
#else
        timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 18000000;
        nanosleep(&ts, NULL);
#endif

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
    }
};

int main (int argc, char *argv[]) {
    std::cout << "Hello World" << std::endl;

    Level *level = new Level();
    level->start();

    level->run();

    delete level;

    SDL_Quit();

    std::cout << "Kill world" << std::endl;
    return 0;
}

