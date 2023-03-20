#pragma once

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Sources
#include <helpers/helpers.hpp>
#include <config/config_handler.hpp>

namespace App {

class Intersector {
public:
    Intersector(std::string intersect_shader_name);
    Intersector(Config::IntersectorConfig config);

    void ClearObstacles();
    void AddObstacles(std::vector<MemoryAlignedBBox> new_obstacle_bboxes);

    void ClearCarParts();
    void AddCarParts(std::vector<MemoryAlignedBBox> new_car_parts_bboxes);

    void Execute();

private:
    std::vector<MemoryAlignedBBox> obstacle_bboxes_;
    std::vector<MemoryAlignedBBox> car_parts_bboxes_;

    std::string intersect_shader_name_;
};

} // namespace App