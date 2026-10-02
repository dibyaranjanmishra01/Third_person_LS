#pragma once

#include "input/InputState.h"
#include "scene/Camera.h"
#include "scene/GameObject.h"

namespace camera
{
    class ThirdPersonCamera
    {
        public:
            explicit ThirdPersonCamera(Camera& camera, GameObject& target);

            void updateCamera(const input::InputState& input, float deltaTime);
        private:
            float _distance{5.0f};
            float _yaw{0.0f};
            float _pitch{0.0f};
            Camera& _camera;
            GameObject& _target;
    };
}
