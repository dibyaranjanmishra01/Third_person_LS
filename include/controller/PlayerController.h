#pragma once

#include "scene/GameObject.h"
#include "input/InputState.h"

namespace controller 
{
    class PlayerController
    {
        public:
            explicit PlayerController(GameObject& player);

            void updatePlayer(const input::InputState& input, const float deltaTime);
        
        private: 
            GameObject& _player; 
    };
}