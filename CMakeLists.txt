cmake_minimum_required(VERSION 3.16)
project(locomotion_system LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(SHADERCROSS_EXE "/home/improfalcon/tools/sdl-shadercross-build/shadercross")

# This assumes the SDL source is available in vendored/SDL
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/glm EXCLUDE_FROM_ALL)

# Create your game executable target as usual
add_executable(locomotion_system 
    src/graphics/Renderer.cpp
    src/graphics/Mesh.cpp
    src/graphics/PrimitiveMeshes.cpp
    src/graphics/ShaderLoader.cpp
    src/app/AppState.cpp
    src/input/InputState.cpp
    src/controller/PlayerController.cpp
    src/main.cpp
)

target_include_directories(locomotion_system PRIVATE include)

target_link_libraries(locomotion_system PRIVATE
    SDL3::SDL3
    glm::glm
)

set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders")

add_custom_command(
    OUTPUT "${SHADER_OUTPUT_DIR}/OnlyPosition.vert.spv"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADER_OUTPUT_DIR}"
    COMMAND "${SHADERCROSS_EXE}"
        "${CMAKE_SOURCE_DIR}/shaders/OnlyPosition.vert.hlsl"
        --source HLSL --dest SPIRV --stage vertex
        --entrypoint main
        --output "${SHADER_OUTPUT_DIR}/OnlyPosition.vert.spv"
    DEPENDS "${CMAKE_SOURCE_DIR}/shaders/OnlyPosition.vert.hlsl"
    VERBATIM
)

add_custom_command(
    OUTPUT "${SHADER_OUTPUT_DIR}/SolidColor.frag.spv"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADER_OUTPUT_DIR}"
    COMMAND "${SHADERCROSS_EXE}"
        "${CMAKE_SOURCE_DIR}/shaders/SolidColor.frag.hlsl"
        --source HLSL --dest SPIRV --stage fragment
        --entrypoint main
        --output "${SHADER_OUTPUT_DIR}/SolidColor.frag.spv"
    DEPENDS "${CMAKE_SOURCE_DIR}/shaders/SolidColor.frag.hlsl"
    VERBATIM
)

add_custom_target(shaders DEPENDS
    "${SHADER_OUTPUT_DIR}/OnlyPosition.vert.spv"
    "${SHADER_OUTPUT_DIR}/SolidColor.frag.spv"
)

add_dependencies(locomotion_system shaders)
