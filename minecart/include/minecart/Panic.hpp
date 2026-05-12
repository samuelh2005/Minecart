#pragma once

#include <spdlog/spdlog.h>
#include <cstdlib>

namespace minecart {

[[noreturn]] inline void panic(const char* message) {
    spdlog::critical("{}", message);
    std::abort();
}

[[noreturn]] inline void panic(const std::string& message) {
    spdlog::critical("{}", message);
    std::abort();
}

} // namespace minecart
