#pragma once

#include <input/InputState.h>

namespace camera
{
    class ThirdPersonCamera
    {
        public:
            explicit ThirdPersonCamera(Camera& camera, GameObject& target);

            void updateCamera(const input::InputState& input, float deltaTime);
        private:
            float _distance{5.0f};
            float yaw{0.0f};
            float pitch{0.0f};
            Camera& _camera;
            GameObject& _target;
    };
}