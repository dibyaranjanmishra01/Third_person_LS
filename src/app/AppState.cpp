#include "app/AppState.h"
#include "graphics/Mesh.h"
#include "graphics/PrimitiveMeshes.h"

#include <array>

namespace appstate{

    AppState::AppState()
    {
        _cube = GameObject{
            .transform = Transform {
                .position = { 0.0f, 1.0f, 5.0f },
                .rotationDegrees = { 20.0f, 35.0f, 0.0f },
                .scale = { 1.0f, 1.0f, 1.0f }
            }
        };

        _cube.mesh = primitives::CreateCube(2.0f);
        _cube.color = { 1.0f, 0.0f, 0.0f, 1.0f };

        _cube2 = GameObject{
            .transform = Transform {
                .position = { 0.7f, 1.0f, 3.0f },
                .rotationDegrees = { 20.0f, 35.0f, 0.0f },
                .scale = { 1.0f, 1.0f, 1.0f }
            }
        };

        _cube2.mesh = primitives::CreateCube(2.0f);
        _cube2.color = { 1.0f, 1.0f, 0.0f, 1.0f };

        _ground = GameObject{
            .transform = Transform {
                .position = { 0.0f, 0.0f, 0.0f },
                .rotationDegrees = { 0.0f, 0.0f, 0.0f },
                .scale = { 1.0f, 1.0f, 1.0f }
            }
        };
        _ground.mesh = primitives::CreatePlane(30.0f, 30.0f);
        _ground.color = { 0.15f, 0.55f, 0.20f, 1.0f };
    }

    AppState::~AppState()
    {
        _renderer.reset();
        _cube.mesh.reset();
        _cube2.mesh.reset();
        _ground.mesh.reset();
        SDL_ReleaseWindowFromGPUDevice(_device, _window);
        SDL_DestroyGPUDevice(_device);
        SDL_DestroyWindow(_window);
    }

    bool AppState::Initialize()
    {
        _window = SDL_CreateWindow("Hello, SDL GPU!", 1280, 720, 0);
        if (_window == nullptr)
        {
            SDL_Log("Couldn't create window: %s", SDL_GetError());
            return false;
        }
        SDL_GPUShaderFormat formatFlags = SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL;
        _device = SDL_CreateGPUDevice(formatFlags, true, nullptr);
        if (_device == nullptr)
        {
            SDL_Log("Couldn't create GPU device: %s", SDL_GetError());
            return false;
        }
        if (!SDL_ClaimWindowForGPUDevice(_device, _window))
        {
            SDL_Log("Couldn't claim window for GPU device: %s", SDL_GetError());
            return false;
        }
        _renderer = std::make_unique<renderer::Renderer>(_device, _window);
        _camera = camera::Camera();
        _camera.position = { 6.0f, 5.0f, -6.0f };
        _camera.forward = glm::normalize(glm::vec3{ 0.0f, 0.0f, 5.0f } - _camera.position);

        if (!_cube.mesh->Initialize(_device) ||
            !_ground.mesh->Initialize(_device) ||
            !_renderer->Initialize() ||
            !_cube2.mesh->Initialize(_device)) 
        {
            return false;
        }
        return true;
    }

    bool AppState::render()
    {
        std::array<GameObject*, 3> objects{
            &_cube,
            &_cube2,
            &_ground,
        };

        return _renderer->render(objects, _camera);
    }
}
