#pragma once

#include <glm/glm.hpp>

#include <memory>

#include "scene/Transform.h"
#include "graphics/Mesh.h"

struct GameObject
{
    Transform transform;
    std::unique_ptr<Mesh> mesh;
    glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
};
