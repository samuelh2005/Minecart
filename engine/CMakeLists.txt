include(FetchContent)

# Build SDL3 as a static library linked straight into the engine, and skip
# the pieces of the SDL build we don't need for this demo. These must be set
# before FetchContent_MakeAvailable() pulls SDL3 in, since they configure
# options inside SDL's own CMakeLists.txt.
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)

# Pinned to the latest full SDL3 release (3.4.12, 2026-07-01) rather than a
# floating branch, so builds stay reproducible until this is bumped on
# purpose.
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY ${MINECART_SDL_REPO}
    GIT_TAG ${MINECART_SDL_RELEASE}
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL3)

# The engine: SDL3, Windows, the Renderer abstraction, and the abstract
# Application. Concrete applications (see examples/) link against this and
# never touch SDL3 directly.
add_library(Minecart STATIC
    src/Application.cpp
    src/Window.cpp
)

target_include_directories(Minecart PUBLIC include)

# SDL's CMake exports SDL3::SDL3 as an ALIAS for whichever of the
# static/shared targets is enabled -- with SDL_SHARED=OFF above, that is the
# static library, so this single link line works whether SDL3::SDL3 resolves
# to SDL3-static or SDL3-shared. PUBLIC because engine's own public headers
# (Application.hpp, Window.hpp, Renderer.hpp) include <SDL3/SDL.h>.
target_link_libraries(Minecart PUBLIC SDL3::SDL3)

if(APPLE)
    target_link_libraries(Minecart PUBLIC "-framework Metal" "-framework QuartzCore")
endif()
