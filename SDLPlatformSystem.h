#pragma once
#include <SDL2/SDL.h>
#include "entityx/entityx.h"
#include "events.h"

class CSDLPlatformSystem : public entityx::System<CSDLPlatformSystem>, public entityx::Receiver<CSDLPlatformSystem>
{
public:
    CSDLPlatformSystem(entityx::Manager *mainloop);
    ~CSDLPlatformSystem();

    void configure(entityx::ptr<entityx::EventManager> events);
    void update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt);
    void receive(const QuitEvent &quit);

private:
    SDL_Window *m_window;
    SDL_GLContext m_context;
    entityx::Manager* m_mainloop;

    float m_w2;
    float m_h2;
};
