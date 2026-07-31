cmake_minimum_required(VERSION 3.24)
project(Minecart LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(MINECART_EXAMPLES ON CACHE BOOL "Build the example applications")
SET(MINECART_SDL_REPO "https://github.com/libsdl-org/SDL.git" CACHE STRING "SDL3 repository to use for builds")
SET(MINECART_SDL_RELEASE "release-3.4.12" CACHE STRING "SDL3 release tag to use for builds")

# The engine owns SDL3, the Window/Renderer abstractions, and the abstract
# Application. Everything under examples/ links against it rather than
# touching SDL directly.
add_subdirectory(engine)

# Concrete applications live under examples/. Each one implements its own
# Application subclass (onInit/onEvent/onTick) and its own Renderer(s).
if(MINECART_EXAMPLES)
    add_subdirectory(examples/color_demo)
    add_subdirectory(examples/window_pong)
endif()
