#pragma once

#include <memory>

#include "graphics/Mesh.h"

namespace primitives
{
    std::unique_ptr<Mesh> CreateCube(float size = 1.0f);
    std::unique_ptr<Mesh> CreatePlane(float width = 10.0f, float depth = 10.0f);
}
