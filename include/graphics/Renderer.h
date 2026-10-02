#pragma once

#include <SDL3/SDL.h>
#include <span>
#include <array>

#include "graphics/Vertex.h"
#include "scene/Camera.h"
#include "scene/GameObject.h"
#include "graphics/Mesh.h"

namespace renderer
{
    class Renderer
    {
    public:
        explicit Renderer(SDL_GPUDevice* device, SDL_Window* window);
        ~Renderer();
        
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        bool Initialize();

        bool render(std::span<GameObject* const> objects, const camera::Camera& camera);
    private:
        bool CreatePipeline();
        bool EnsureDepthTexture(int width, int height);


        SDL_GPUDevice* _device = nullptr;
        SDL_Window* _window = nullptr;
        SDL_GPUGraphicsPipeline* _pipeline = nullptr;
        SDL_GPUTexture* _depthTexture = nullptr;
        int _depthTextureWidth = 0;
        int _depthTextureHeight = 0;
        static constexpr SDL_GPUTextureFormat DepthFormat =
            SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
    };
}
