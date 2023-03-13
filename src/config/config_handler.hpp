#pragma once

// STL
#include <fstream>

// JSON
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace App {

class ConfigHandler {
public:
    ConfigHandler(std::string filename) {
        std::ifstream input{filename};
        json data = json::parse(input);
    }

private:
    
};

} // namespace App