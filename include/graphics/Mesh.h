#pragma once

#include <SDL3/SDL_gpu.h>

#include <vector>

#include "graphics/Vertex.h"

class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<Uint16> indices = {});
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    bool Initialize(SDL_GPUDevice* device);
    std::size_t GetVertexCount() const;
    std::size_t GetIndexCount() const;
    SDL_GPUBuffer* GetVertexBuffer() const;
    SDL_GPUBuffer* GetIndexBuffer() const;
    bool HasIndices() const;

private:
    SDL_GPUDevice* _device = nullptr;
    std::vector<Vertex> _vertices;
    std::vector<Uint16> _indices;
    SDL_GPUBuffer* _vertexBuffer = nullptr;
    SDL_GPUBuffer* _indexBuffer = nullptr;
};
