#include "input/InputState.h"

#include <SDL3/SDL.h>

namespace input 
{
    InputState::InputState(const bool* keyboard,float mouseDeltaX,float mouseDeltaY) :
    forward{static_cast<float>(keyboard[SDL_SCANCODE_W])},
    backward{static_cast<float>(keyboard[SDL_SCANCODE_S])},
    left{static_cast<float>(keyboard[SDL_SCANCODE_A])},
    right{static_cast<float>(keyboard[SDL_SCANCODE_D])},
    jump{keyboard[SDL_SCANCODE_SPACE]},
    mouseDeltaX{mouseDeltaX},
    mouseDeltaY{mouseDeltaY}
    {}

    void InputState::debugPrintInputState() const
    {
        std::cout<<"forward"<<" "<<forward<<"\n";
        std::cout<<"backward"<<" "<<backward<<"\n";
        std::cout<<"left"<<" "<<left<<"\n";
        std::cout<<"right"<<" "<<right<<"\n";
        std::cout<<"jump"<<" "<<jump<<"\n";
    }

    float InputState::getForward() const
    {
        return forward;
    }

    float InputState::getBackword() const
    {
        return backward;
    }

    float InputState::getLeft() const
    {
        return left;
    }

    float InputState::getRight() const
    {
        return right;
    }

    bool InputState::getJump() const
    {
        return jump;
    }

    float InputState::getMouseDeltaX() const
    {
        return mouseDeltaX;
    }

    float InputState::getMouseDeltaY() const
    {
        return mouseDeltaY;
    }
}
