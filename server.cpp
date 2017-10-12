#include <iostream>
#include <string>
#include <time.h>
#include <SDL2/SDL.h>
#include "entityx/entityx.h"
#include "components.h"
#include "events.h"
#include "systems.h"

int id = 4;

#define DEBUG_NETWORK
#define DEBUG_ENTITY
#define DEBUG_INPUT

class Level : public entityx::Manager {
public:
    explicit Level() { }

protected:
    void configure() {
        system_manager->add<scripting::CLuaSystem>(entity_manager, event_manager);

        system_manager->add<physics::CBox2dPhysicsSystem>(entity_manager, event_manager);

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
        entry.assign<Script>("scripts/bootstrap.lua", "server");
    }

    void update(double dt) {
        system_manager->update<CPlayerSystem>(dt);
   
        system_manager->update<physics::CBox2dPhysicsSystem>(dt);

        system_manager->update<scripting::CLuaSystem>(dt);

        system_manager->update<CENetNetworkSystem>(dt);

#ifdef DEBUG_PHYSICS
        system_manager->update<CDebugPhysicsSystem>(dt);
#endif
#ifdef DEBUG_NETWORK
        system_manager->update<DebugNetworkSystem>(dt);
#endif
#ifdef DEBUG_ENTITY
        system_manager->update<DebugEntitySystem>(dt);
#endif
#ifdef DEBUG_PLAYER
        system_manager->update<CDebugPlayerSystem>(dt);
#endif

        timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 18000000;
        nanosleep(&ts, NULL);
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

