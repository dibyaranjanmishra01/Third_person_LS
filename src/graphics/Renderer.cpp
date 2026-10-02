#include <span>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/Renderer.h"
#include "graphics/ShaderLoader.h"

namespace renderer
{
    Renderer::Renderer(SDL_GPUDevice* device, SDL_Window* window) : 
    _device(device),
    _window(window)
    {
    }

    bool Renderer::Initialize()
    {
        if (!CreatePipeline())
        {
            return false;
        }
        return true;
    }

    Renderer::~Renderer()
    {
        if (_depthTexture != nullptr)
        {
            SDL_ReleaseGPUTexture(_device, _depthTexture);
            _depthTexture = nullptr;
        }

        if (_pipeline != nullptr)
        {
            SDL_ReleaseGPUGraphicsPipeline(_device, _pipeline);
            _pipeline = nullptr;
        }
    }

    bool Renderer::CreatePipeline()
    {
        SDL_GPUShader* vertexShader = shaders::Load(_device, "OnlyPosition.vert");
        if (vertexShader == nullptr)
        {
            SDL_Log("Couldn't create vertex shader!");
            return false;
        }

        SDL_GPUShader* fragmentShader = shaders::Load(_device, "SolidColor.frag");
        if (fragmentShader == nullptr)
        {
            SDL_Log("Couldn't create fragment shader!");
            SDL_ReleaseGPUShader(_device, vertexShader);
            return false;
        }

        std::array vertexBufferDescriptions{
            SDL_GPUVertexBufferDescription{
                .slot = 0,
                .pitch = sizeof(Vertex),
                .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                .instance_step_rate = 0,
            },
        };

        std::array vertexAttributes{
            SDL_GPUVertexAttribute{
                .location = 0,
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT,
                .offset = 0 * sizeof(float),
            },
            SDL_GPUVertexAttribute{
                .location = 1,
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT,
                .offset = 1 * sizeof(float),
            },
            SDL_GPUVertexAttribute{
                .location = 2,
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT,
                .offset = 2 * sizeof(float),
            },
        };

        std::array colorTargetDescriptions{
            SDL_GPUColorTargetDescription{
                .format = SDL_GetGPUSwapchainTextureFormat(_device, _window)
            }
        };

        SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = SDL_GPUGraphicsPipelineCreateInfo{
            .vertex_shader = vertexShader,
            .fragment_shader = fragmentShader,
            .vertex_input_state = SDL_GPUVertexInputState{
                .vertex_buffer_descriptions = vertexBufferDescriptions.data(),
                .num_vertex_buffers = vertexBufferDescriptions.size(),
                .vertex_attributes = vertexAttributes.data(),
                .num_vertex_attributes = vertexAttributes.size(),
            },
            .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
            .rasterizer_state = SDL_GPURasterizerState{
                .fill_mode = SDL_GPU_FILLMODE_FILL,
            },
            .depth_stencil_state = SDL_GPUDepthStencilState{
                .compare_op = SDL_GPU_COMPAREOP_LESS_OR_EQUAL,
                .enable_depth_test = true,
                .enable_depth_write = true,
            },
            .target_info = SDL_GPUGraphicsPipelineTargetInfo{
                .color_target_descriptions = colorTargetDescriptions.data(),
                .num_color_targets = colorTargetDescriptions.size(),
                .depth_stencil_format = DepthFormat,
                .has_depth_stencil_target = true,
            },
        };

        _pipeline = SDL_CreateGPUGraphicsPipeline(_device, &pipelineCreateInfo);
        SDL_ReleaseGPUShader(_device, vertexShader);
        SDL_ReleaseGPUShader(_device, fragmentShader);
        if (_pipeline == nullptr)
        {
            SDL_Log("Couldn't create graphics pipeline! %s", SDL_GetError());
            return false;
        }

        return true;
    }

    bool Renderer::EnsureDepthTexture(int width, int height)
    {
        if (width <= 0 || height <= 0)
        {
            return false;
        }

        if (_depthTexture != nullptr &&
            _depthTextureWidth == width &&
            _depthTextureHeight == height)
        {
            return true;
        }

        if (_depthTexture != nullptr)
        {
            SDL_ReleaseGPUTexture(_device, _depthTexture);
            _depthTexture = nullptr;
        }

        SDL_GPUTextureCreateInfo depthTextureInfo{
            .type = SDL_GPU_TEXTURETYPE_2D,
            .format = DepthFormat,
            .usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
            .width = static_cast<Uint32>(width),
            .height = static_cast<Uint32>(height),
            .layer_count_or_depth = 1,
            .num_levels = 1,
            .sample_count = SDL_GPU_SAMPLECOUNT_1,
        };
        _depthTexture = SDL_CreateGPUTexture(_device, &depthTextureInfo);
        if (_depthTexture == nullptr)
        {
            SDL_Log("Couldn't create depth texture: %s", SDL_GetError());
            return false;
        }

        _depthTextureWidth = width;
        _depthTextureHeight = height;
        return true;
    }

    bool Renderer::render(std::span<GameObject* const> objects, const camera::Camera& camera)
    {
        int width = 0;
        int height = 0;
        SDL_GetWindowSizeInPixels(_window, &width, &height);
        if (width <= 0 || height <= 0)
        {
            return true;
        }

        if (!EnsureDepthTexture(width, height))
        {
            return false;
        }

        SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(_device);
        if (commandBuffer == nullptr)
        {
            SDL_Log("Couldn't acquire GPU command buffer: %s", SDL_GetError());
            return false;
        }
        SDL_GPUTexture* swapchainTexture = nullptr;
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, _window, &swapchainTexture, nullptr, nullptr))
        {
            SDL_Log("Couldn't acquire swapchain texture: %s", SDL_GetError());
            SDL_CancelGPUCommandBuffer(commandBuffer);
            return false;
        }

        if (swapchainTexture == nullptr)
        {
            if (!SDL_SubmitGPUCommandBuffer(commandBuffer))
            {
                SDL_Log("Couldn't submit GPU command buffer: %s", SDL_GetError());
                return false;
            }

            return true;
        }

        SDL_GPUColorTargetInfo colorTargetInfo = { 0 };
        colorTargetInfo.texture = swapchainTexture;
        colorTargetInfo.clear_color = (SDL_FColor){ 0.4f, 0.6f, 0.9f, 1.0f };
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPUDepthStencilTargetInfo depthTargetInfo{
            .texture = _depthTexture,
            .clear_depth = 1.0f,
            .load_op = SDL_GPU_LOADOP_CLEAR,
            .store_op = SDL_GPU_STOREOP_DONT_CARE,
        };

        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, &depthTargetInfo);

        SDL_BindGPUGraphicsPipeline(renderPass, _pipeline);

        float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix(aspectRatio);

        for (const GameObject* object : objects)
        {
            if (object == nullptr || !object->mesh)
            {
                continue;
            }

            std::array vertexBuffers{
                SDL_GPUBufferBinding{
                    .buffer = object->mesh->GetVertexBuffer(),
                    .offset = 0,
                },
            };
            
            SDL_BindGPUVertexBuffers(renderPass, 0, vertexBuffers.data(), 1);

            glm::mat4 model = object->transform.GetModelMatrix();
            glm::mat4 mvp = projection * view * model;
            SDL_PushGPUVertexUniformData(
                commandBuffer,
                0,              // matches b0
                &mvp,
                sizeof(mvp)
            );
            SDL_PushGPUFragmentUniformData(
                commandBuffer,
                0,
                &object->color,
                sizeof(object->color)
            );
            if (object->mesh->HasIndices())
            {
                SDL_GPUBufferBinding indexBuffer{
                    .buffer = object->mesh->GetIndexBuffer(),
                    .offset = 0,
                };
                SDL_BindGPUIndexBuffer(renderPass, &indexBuffer, SDL_GPU_INDEXELEMENTSIZE_16BIT);
                SDL_DrawGPUIndexedPrimitives(
                    renderPass,
                    static_cast<Uint32>(object->mesh->GetIndexCount()),
                    1,
                    0,
                    0,
                    0
                );
            }
            else
            {
                SDL_DrawGPUPrimitives(renderPass, static_cast<Uint32>(object->mesh->GetVertexCount()), 1, 0, 0);
            }
        }

        SDL_EndGPURenderPass(renderPass);

        if (!SDL_SubmitGPUCommandBuffer(commandBuffer)) {
            SDL_Log("Couldn't submit GPU command buffer: %s", SDL_GetError());
            return false;
        }
        return true;
    }
}
