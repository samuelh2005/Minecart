add_executable(window_pong
    src/main.cpp
    src/WindowPongApplication.cpp
    src/ClearColorRenderer.cpp
)

target_include_directories(window_pong PRIVATE src)

target_link_libraries(window_pong PRIVATE engine)
