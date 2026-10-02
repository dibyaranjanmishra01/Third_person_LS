#include "controller/PlayerController.h"
#include <iostream>

namespace controller
{
    PlayerController::PlayerController(GameObject& player) :
    _player{player}
    {}

    void PlayerController::updatePlayer(const input::InputState& input, const float deltaTime)
    {
        if(input.getForward())
        {
            _player.transform.position.z += (1.0f * deltaTime);
        }
    }
}