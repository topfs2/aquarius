#include "InputSystem.h"
#include "Log.h"

using namespace logging;

void CInputSystem::receive(const KeyEvent &e) {
    if (e.device.compare("keyboard") == 0) {
        if (e.state && e.key.compare("Q") == 0) {
            m_events->emit<QuitEvent>(0);
        }
    }
}
