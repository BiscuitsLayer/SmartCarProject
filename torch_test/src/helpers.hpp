#pragma once

#include <fstream>

bool CheckFileExists(const std::string& path) {
    std::ifstream file{path};
    return file.good();
}