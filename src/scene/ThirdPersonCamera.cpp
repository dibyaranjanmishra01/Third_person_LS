#include "scene/ThirdPersonCamera.h"

#include <algorithm>
#include <cmath>

#include <glm/glm.hpp>

namespace camera
{
    ThirdPersonCamera::ThirdPersonCamera(Camera& camera, GameObject& target) :
        _camera{camera},
        _target{target}
    {}

    void ThirdPersonCamera::updateCamera(const input::InputState& input, float deltaTime)
    {
        // Mouse deltas already describe the motion since the previous frame, so
        // multiplying them by deltaTime would make rotation frame-rate dependent.
        static_cast<void>(deltaTime);

        constexpr float mouseSensitivity = 0.003f;
        constexpr float pitchLimit = glm::radians(85.0f);

        _yaw += input.getMouseDeltaX() * mouseSensitivity;
        _pitch = std::clamp(
            _pitch - input.getMouseDeltaY() * mouseSensitivity,
            -pitchLimit,
            pitchLimit
        );

        const float cosPitch = std::cos(_pitch);
        const glm::vec3 offset{
            _distance * cosPitch * std::sin(_yaw),
            _distance * std::sin(_pitch),
            -_distance * cosPitch * std::cos(_yaw)
        };

        _camera.position = _target.transform.position + offset;
        _camera.forward = glm::normalize(_target.transform.position - _camera.position);
        _camera.up = {0.0f, 1.0f, 0.0f};
    }
}
