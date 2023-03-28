#pragma once

// Assimp
#include <assimp/scene.h>

namespace App {

using AssimpParameterType = std::pair<AssimpMaterialTextureParameters, AssimpMaterialColorParameters>;

class AssimpLoader;

} // namespace App