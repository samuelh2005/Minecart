cmake_minimum_required(VERSION 3.24)
project(Minecart LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# The engine owns SDL3, the Window/Renderer abstractions, and the abstract
# Application. Everything under examples/ links against it rather than
# touching SDL directly.
add_subdirectory(engine)

# Concrete applications live under examples/. Each one implements its own
# Application subclass (onInit/onEvent/onTick) and its own Renderer(s).
add_subdirectory(examples/color_demo)
