#include "app/AppState.h"
#include "graphics/Mesh.h"
#include "graphics/PrimitiveMeshes.h"
#include "input/InputState.h"

#include <SDL3/SDL.h>


#include <array>

namespace{
    float getDeltaTime(Uint64& previousTime)
    {
        const Uint64 currentTime = SDL_GetTicks();
        const float deltaTime = (currentTime - previousTime) / 1000.0f;
        previousTime = currentTime;
        return std::min(deltaTime, 0.1f);
    }
}

namespace appstate{

    AppState::AppState() :
    _playerController{_cube}
    {
        _cube = GameObject{
            .transform = Transform {
                .position = { 0.0f, 1.0f, 0.0f },
                .rotationDegrees = { 0.0f, 0.0f, 0.0f },
                .scale = { 1.0f, 1.0f, 1.0f }
            }
        };

        _cube.mesh = primitives::CreateCube(2.0f);
        _cube.color = { 1.0f, 1.0f, 1.0f, 1.0f };

        _ground = GameObject{
            .transform = Transform {
                .position = { 0.0f, 0.0f, 0.0f },
                .rotationDegrees = { 0.0f, 0.0f, 0.0f },
                .scale = { 1.0f, 1.0f, 1.0f }
            }
        };
        _ground.mesh = primitives::CreatePlane(30.0f, 30.0f);
        _ground.color = { 0.15f, 0.55f, 0.20f, 1.0f };

        previousTime = SDL_GetTicks();
    }

    AppState::~AppState()
    {
        _renderer.reset();
        _cube.mesh.reset();
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
        _keyboard = SDL_GetKeyboardState(nullptr);
        _renderer = std::make_unique<renderer::Renderer>(_device, _window);
        _camera = camera::Camera();
        _camera.position = { 6.0f, 5.0f, -6.0f };
        _camera.forward = glm::normalize(glm::vec3{ 0.0f, 0.0f, 5.0f } - _camera.position);

        if (!_cube.mesh->Initialize(_device) ||
            !_ground.mesh->Initialize(_device) ||
            !_renderer->Initialize()) 
        {
            return false;
        }
        return true;
    }

    void AppState::addMouseInput(float xrel, float yrel)
    {
        _mouseDeltaX += xrel;
        _mouseDeltaY += yrel;
    }

    bool AppState::loop()
    {
        std::array<GameObject*, 2> objects{
            &_cube,
            &_ground,
        };

        const float deltaTime = getDeltaTime(previousTime); 

        _playerController.updatePlayer(input::InputState{_keyboard, _mouseDeltaX, _mouseDeltaY}, deltaTime);

        return _renderer->render(objects, _camera);
    }
}
