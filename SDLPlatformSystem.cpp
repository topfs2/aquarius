#include "SDLPlatformSystem.h"
#include <GL/glew.h>
#include <sstream>
#include "settings.h"

CSDLPlatformSystem::CSDLPlatformSystem(entityx::Manager *mainloop) : m_mainloop(mainloop) {
    SDL_Init(SDL_INIT_VIDEO);

    m_window = SDL_CreateWindow( 
        "Aquarius",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
    );
    m_context = SDL_GL_CreateContext(m_window);

    m_w2 = WINDOW_WIDTH / 2.0f;
    m_h2 = WINDOW_HEIGHT / 2.0f;

    glewInit();
}

CSDLPlatformSystem::~CSDLPlatformSystem() {
    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window); 
}

void CSDLPlatformSystem::configure(entityx::ptr<entityx::EventManager> events) {
    events->subscribe<QuitEvent>(*this);
}

void CSDLPlatformSystem::update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt) {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        std::stringstream device;
        std::stringstream key;

        switch (event.type)
        {
            case SDL_MOUSEMOTION:
                device << "mouse-" << (int)event.motion.which;
                key << "mouse";

                events->emit<MotionEvent>(device.str(), key.str(), ((float)event.motion.x - m_w2) / m_w2, ((float)event.motion.y - m_h2) / m_h2);
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                device << "mouse-" << (int)event.button.which;
                key << (int)event.button.button;

                events->emit<KeyEvent>(event.type == SDL_MOUSEBUTTONDOWN, device.str(), key.str());
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                device << "keyboard";
                key << SDL_GetKeyName(event.key.keysym.sym);

                if (event.key.repeat == 0) {
                    events->emit<KeyEvent>(event.type == SDL_KEYDOWN, device.str(), key.str());
                }
                break;

            case SDL_QUIT:
                events->emit<QuitEvent>(0);
                break;
        }
    }

    SDL_GL_SwapWindow(m_window);
}

void CSDLPlatformSystem::receive(const QuitEvent &quit) {
    m_mainloop->stop();
}
