#pragma once

#include <SDL3/SDL_gpu.h>
#include <string>

namespace shaders
{
    SDL_GPUShader* Load(SDL_GPUDevice* device, const std::string& shaderFilename);
}