add_library(minecart
    src/Application.cpp
    src/JsonResource.cpp
    src/FileSystemResourceLoader.cpp
    src/ResourceManager.cpp
    src/Localisation.cpp
    src/Window.cpp
)

target_include_directories(minecart
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(minecart
    PUBLIC
        spdlog::spdlog
        imgui
        SDL3::SDL3
        nlohmann_json::nlohmann_json
)

target_compile_features(minecart PUBLIC cxx_std_20)
