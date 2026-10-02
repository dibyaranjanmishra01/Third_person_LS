#include "graphics/Mesh.h"

#include <SDL3/SDL.h>

#include <limits>
#include <utility>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Uint16> indices)
    : _vertices(std::move(vertices))
    , _indices(std::move(indices))
{
}

Mesh::~Mesh()
{
    if (_indexBuffer != nullptr)
    {
        SDL_ReleaseGPUBuffer(_device, _indexBuffer);
        _indexBuffer = nullptr;
    }

    if (_vertexBuffer != nullptr)
    {
        SDL_ReleaseGPUBuffer(_device, _vertexBuffer);
        _vertexBuffer = nullptr;
    }
}

std::size_t Mesh::GetVertexCount() const
{
    return _vertices.size();
}

std::size_t Mesh::GetIndexCount() const
{
    return _indices.size();
}

bool Mesh::Initialize(SDL_GPUDevice* device)
{
    if (device == nullptr || _vertices.empty())
    {
        SDL_Log("Cannot initialize an empty mesh or use a null GPU device.");
        return false;
    }

    if (GetVertexCount() > std::numeric_limits<Uint32>::max() / sizeof(Vertex) ||
        GetIndexCount() > std::numeric_limits<Uint32>::max() / sizeof(Uint16))
    {
        SDL_Log("Mesh vertex data is too large for an SDL GPU buffer.");
        return false;
    }

    _device = device;
    const Uint32 verticesSize = static_cast<Uint32>(GetVertexCount() * sizeof(Vertex));
    SDL_GPUBufferCreateInfo vertexBufferCreateInfo = SDL_GPUBufferCreateInfo{
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = verticesSize,
    };
    _vertexBuffer = SDL_CreateGPUBuffer(device, &vertexBufferCreateInfo);
    if (_vertexBuffer == nullptr)
    {
        SDL_Log("Couldn't create vertex buffer: %s", SDL_GetError());
        return false;
    }

    const Uint32 indexBytes = static_cast<Uint32>(GetIndexCount() * sizeof(Uint16));
    if (HasIndices())
    {
        SDL_GPUBufferCreateInfo indexBufferCreateInfo{
            .usage = SDL_GPU_BUFFERUSAGE_INDEX,
            .size = indexBytes,
        };
        _indexBuffer = SDL_CreateGPUBuffer(_device, &indexBufferCreateInfo);
        if (_indexBuffer == nullptr)
        {
            SDL_Log("Couldn't create index buffer: %s", SDL_GetError());
            SDL_ReleaseGPUBuffer(_device, _vertexBuffer);
            _vertexBuffer = nullptr;
            return false;
        }
    }

    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = SDL_GPUTransferBufferCreateInfo{
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = verticesSize,
    };
    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(_device, &transferBufferCreateInfo);
    if (transferBuffer == nullptr)
    {
        SDL_Log("Couldn't create transfer buffer: %s", SDL_GetError());
        if (_indexBuffer != nullptr)
        {
            SDL_ReleaseGPUBuffer(_device, _indexBuffer);
            _indexBuffer = nullptr;
        }
        SDL_ReleaseGPUBuffer(_device, _vertexBuffer);
        _vertexBuffer = nullptr;
        return false;
    }

    Vertex* transferData = static_cast<Vertex*>(SDL_MapGPUTransferBuffer(_device, transferBuffer, false));
    if (transferData == nullptr)
    {
        SDL_Log("Couldn't map transfer buffer: %s", SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(_device, transferBuffer);
        if (_indexBuffer != nullptr)
        {
            SDL_ReleaseGPUBuffer(_device, _indexBuffer);
            _indexBuffer = nullptr;
        }
        SDL_ReleaseGPUBuffer(_device, _vertexBuffer);
        _vertexBuffer = nullptr;
        return false;
    }

    SDL_memcpy(transferData, _vertices.data(), verticesSize);
    SDL_UnmapGPUTransferBuffer(_device, transferBuffer);

    SDL_GPUTransferBuffer* indexTransferBuffer = nullptr;
    if (HasIndices())
    {
        SDL_GPUTransferBufferCreateInfo indexTransferBufferCreateInfo{
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size = indexBytes,
        };
        indexTransferBuffer = SDL_CreateGPUTransferBuffer(_device, &indexTransferBufferCreateInfo);
        if (indexTransferBuffer == nullptr)
        {
            SDL_Log("Couldn't create index transfer buffer: %s", SDL_GetError());
            SDL_ReleaseGPUTransferBuffer(_device, transferBuffer);
            SDL_ReleaseGPUBuffer(_device, _indexBuffer);
            SDL_ReleaseGPUBuffer(_device, _vertexBuffer);
            _indexBuffer = nullptr;
            _vertexBuffer = nullptr;
            return false;
        }

        void* indexTransferData = SDL_MapGPUTransferBuffer(_device, indexTransferBuffer, false);
        if (indexTransferData == nullptr)
        {
            SDL_Log("Couldn't map index transfer buffer: %s", SDL_GetError());
            SDL_ReleaseGPUTransferBuffer(_device, indexTransferBuffer);
            SDL_ReleaseGPUTransferBuffer(_device, transferBuffer);
            SDL_ReleaseGPUBuffer(_device, _indexBuffer);
            SDL_ReleaseGPUBuffer(_device, _vertexBuffer);
            _indexBuffer = nullptr;
            _vertexBuffer = nullptr;
            return false;
        }

        SDL_memcpy(indexTransferData, _indices.data(), indexBytes);
        SDL_UnmapGPUTransferBuffer(_device, indexTransferBuffer);
    }

    SDL_GPUCommandBuffer* uploadCommandBuffer = SDL_AcquireGPUCommandBuffer(_device);
    if (uploadCommandBuffer == nullptr)
    {
        SDL_Log("Couldn't acquire GPU command buffer: %s", SDL_GetError());
        if (indexTransferBuffer != nullptr)
        {
            SDL_ReleaseGPUTransferBuffer(_device, indexTransferBuffer);
        }
        SDL_ReleaseGPUTransferBuffer(_device, transferBuffer);
        if (_indexBuffer != nullptr)
        {
            SDL_ReleaseGPUBuffer(_device, _indexBuffer);
            _indexBuffer = nullptr;
        }
        SDL_ReleaseGPUBuffer(_device, _vertexBuffer);
        _vertexBuffer = nullptr;
        return false;
    }

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCommandBuffer);
    SDL_GPUTransferBufferLocation source{
        .transfer_buffer = transferBuffer,
        .offset = 0,
    };
    SDL_GPUBufferRegion destination{
        .buffer = _vertexBuffer,
        .offset = 0,
        .size = verticesSize,
    };
    SDL_UploadToGPUBuffer(copyPass, &source, &destination, false);
    if (HasIndices())
    {
        SDL_GPUTransferBufferLocation indexSource{
            .transfer_buffer = indexTransferBuffer,
            .offset = 0,
        };
        SDL_GPUBufferRegion indexDestination{
            .buffer = _indexBuffer,
            .offset = 0,
            .size = indexBytes,
        };
        SDL_UploadToGPUBuffer(copyPass, &indexSource, &indexDestination, false);
    }
    SDL_EndGPUCopyPass(copyPass);

    if (!SDL_SubmitGPUCommandBuffer(uploadCommandBuffer))
    {
        SDL_Log("Couldn't submit GPU command buffer: %s", SDL_GetError());
        if (indexTransferBuffer != nullptr)
        {
            SDL_ReleaseGPUTransferBuffer(_device, indexTransferBuffer);
        }
        SDL_ReleaseGPUTransferBuffer(_device, transferBuffer);
        if (_indexBuffer != nullptr)
        {
            SDL_ReleaseGPUBuffer(_device, _indexBuffer);
            _indexBuffer = nullptr;
        }
        SDL_ReleaseGPUBuffer(_device, _vertexBuffer);
        _vertexBuffer = nullptr;
        return false;
    }

    if (indexTransferBuffer != nullptr)
    {
        SDL_ReleaseGPUTransferBuffer(_device, indexTransferBuffer);
    }
    SDL_ReleaseGPUTransferBuffer(_device, transferBuffer);
    return true;
}

SDL_GPUBuffer* Mesh::GetVertexBuffer() const
{
    return _vertexBuffer;
}

SDL_GPUBuffer* Mesh::GetIndexBuffer() const
{
    return _indexBuffer;
}

bool Mesh::HasIndices() const
{
    return !_indices.empty();
}
