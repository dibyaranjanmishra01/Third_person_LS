#pragma once

#include <SDL3/SDL.h>
#include <memory>

#include "graphics/Renderer.h"
#include "scene/Camera.h"

// struct MyAppState
// {
// 	SDL_Window* window = nullptr;
//     SDL_GPUDevice* device = nullptr;
//     SDL_GPUGraphicsPipeline* pipeline = nullptr;
//     // Uint32 numVertices = 0;
//     // SDL_GPUBuffer* vertexBuffer = nullptr;
//     Uint64 previousTimeNs = 0; //time elapsed since start of application in ns 
//     renderer::Renderer* renderer = nullptr;
// };

namespace appstate{
    class AppState {
        public :
            AppState();
            bool render();
            bool Initialize();
            ~AppState();

        private:
            SDL_Window* _window = nullptr;
            SDL_GPUDevice* _device = nullptr;
            Uint64 previousTimeNs = 0;
            std::unique_ptr<renderer::Renderer> _renderer;
            camera::Camera _camera;

            //-----------
            GameObject _cube;
            GameObject _cube2;
            GameObject _ground;
    };
}
