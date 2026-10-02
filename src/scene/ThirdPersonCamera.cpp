#include "ThirdPersonCamera.h"

namespace camera
{
    ThirdPersonCamera::ThirdPersonCamera(Camera& camera, GameObject& target):
    _camera{camera}
    _target{target}
    {}

    ThirdPersonCamera::updateCamera(const input::InputState& input, float deltaTime)
    {
        
    }
}