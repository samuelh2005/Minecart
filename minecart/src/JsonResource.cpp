#include "minecart/resource/JsonResource.hpp"
#include <fstream>

namespace minecart::resource {

json readJsonFile(const std::filesystem::path& file) {
    std::ifstream f(file);
    json data = json::parse(f);
    return data;
}

} // namespace minecart::resource
