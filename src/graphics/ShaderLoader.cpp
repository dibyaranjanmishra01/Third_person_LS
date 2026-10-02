#include "graphics/ShaderLoader.h"

#include <SDL3/SDL.h>
#include <filesystem>
#include <string>

namespace shaders {
    SDL_GPUShader* Load(SDL_GPUDevice* device, const std::string& shaderFilename)
    {
        SDL_GPUShaderStage stage;
        if(shaderFilename.contains(".vert"))
        {
            stage = SDL_GPU_SHADERSTAGE_VERTEX;

        }
        else if (shaderFilename.contains(".frag"))
        {
            stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
        }
        else{
            SDL_Log("Couldn't deduce shader stage from file name: %s", shaderFilename.c_str());
            return nullptr;
        }

        std::filesystem::path fullPath = std::filesystem::path(SDL_GetBasePath()) / ".." / "shaders";

        SDL_Log("BAse file path: %s", fullPath.string().c_str());
        SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
        const char* entrypoint;

        SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(device);
        if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV)
        {
            fullPath /= shaderFilename + ".spv";
            format = SDL_GPU_SHADERFORMAT_SPIRV;
            entrypoint = "main";
        }
        else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL)
        {
            fullPath /= shaderFilename + ".msl";
            format = SDL_GPU_SHADERFORMAT_MSL;
            entrypoint = "main0";
        }
        else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL)
        {
            fullPath /= shaderFilename + ".dxil";
            format = SDL_GPU_SHADERFORMAT_DXIL;
            entrypoint = "main";
        }
        else
        {
            SDL_Log("Couldn't find a supported shader format for backend %s!", SDL_GetGPUDeviceDriver(device));
            return nullptr;
        }

        size_t fileSize;
        void* code = SDL_LoadFile(fullPath.string().c_str(), &fileSize);
        if (code == nullptr)
        {
            SDL_Log(
                "Couldn't load shader file '%s': %s",
                fullPath.string().c_str(),
                SDL_GetError()
            );
            return nullptr;
        }

        SDL_GPUShaderCreateInfo shaderInfo = SDL_GPUShaderCreateInfo{
            .code_size = fileSize,
            .code = static_cast<Uint8*>(code),
            .entrypoint = entrypoint,
            .format = format,
            .stage = stage,
            .num_uniform_buffers = 1
        };
        SDL_GPUShader* shader = SDL_CreateGPUShader(device, &shaderInfo);
        if (shader == nullptr)
        {
            SDL_Log("Couldn't create shader from file %s: %s", fullPath.c_str(), SDL_GetError());
            SDL_free(code);
            return nullptr;
        }
        SDL_free(code);
        return shader;
    }
}
