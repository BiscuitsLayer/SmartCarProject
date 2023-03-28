#pragma once

// STL
#include <array>

namespace App {

enum class KeyboardMode: int {
    ORBIT_CAMERA = 0,
    FIRST_PERSON_CAMERA,
    CAR_MOVEMENT,
    SIZE
};

using KeyboardStatus = std::array<bool, APP_KEYBOARD_KEYS_COUNT>;
using ShaderHandler = std::unordered_map<std::string, std::shared_ptr<GL::Program>>;

struct MemoryAlignedBBox;
struct Context;

} // namespace App