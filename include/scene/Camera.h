#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace camera
{
    struct Camera
    {
        // SDL GPU uses a left-handed clip-space convention.
        glm::vec3 position{ 0.0f, 0.0f, -5.0f };
        glm::vec3 forward{ 0.0f, 0.0f, 1.0f };
        glm::vec3 up{ 0.0f, 1.0f, 0.0f };

        float fieldOfViewDegrees = 60.0f;
        float nearPlane = 0.1f;
        float farPlane = 100.0f;

        glm::mat4 GetViewMatrix() const
        {
            return glm::lookAtLH(
                position,
                position + forward,
                up
            );
        }

        glm::mat4 GetProjectionMatrix(float aspectRatio) const
        {
            return glm::perspectiveLH_ZO(
                glm::radians(fieldOfViewDegrees),
                aspectRatio,
                nearPlane,
                farPlane
            );
        }
    };
}