# Example application built on top of the engine: a concrete Application
# subclass plus two concrete Renderer subclasses, and nothing else.
add_executable(color_demo
    src/main.cpp
    src/ColorDemoApplication.cpp
    src/ClearColorRenderer.cpp
    src/PulsingClearColorRenderer.cpp
)

target_include_directories(color_demo PRIVATE src)

target_link_libraries(color_demo PRIVATE Minecart)
