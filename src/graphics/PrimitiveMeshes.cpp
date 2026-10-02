#include "graphics/PrimitiveMeshes.h"

namespace primitives
{
    std::unique_ptr<Mesh> CreateCube(float size)
    {
        const float halfSize = size * 0.5f;

        return std::make_unique<Mesh>(
            std::vector<Vertex>{
                {-halfSize, -halfSize, -halfSize},
                { halfSize, -halfSize, -halfSize},
                { halfSize,  halfSize, -halfSize},
                {-halfSize,  halfSize, -halfSize},
                {-halfSize, -halfSize,  halfSize},
                { halfSize, -halfSize,  halfSize},
                { halfSize,  halfSize,  halfSize},
                {-halfSize,  halfSize,  halfSize},
            },
            std::vector<Uint16>{
                4, 5, 6, 4, 6, 7, // front
                0, 2, 1, 0, 3, 2, // back
                0, 7, 3, 0, 4, 7, // left
                1, 2, 6, 1, 6, 5, // right
                3, 7, 6, 3, 6, 2, // top
                0, 1, 5, 0, 5, 4, // bottom
            }
        );
    }

    std::unique_ptr<Mesh> CreatePlane(float width, float depth)
    {
        const float halfWidth = width * 0.5f;
        const float halfDepth = depth * 0.5f;

        return std::make_unique<Mesh>(
            std::vector<Vertex>{
                {-halfWidth, 0.0f, -halfDepth},
                { halfWidth, 0.0f, -halfDepth},
                { halfWidth, 0.0f,  halfDepth},
                {-halfWidth, 0.0f,  halfDepth},
            },
            std::vector<Uint16>{
                0, 1, 2,
                0, 2, 3,
            }
        );
    }
}
