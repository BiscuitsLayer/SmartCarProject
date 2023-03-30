#pragma once

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Constants
#include <constants/constants.hpp>

// Forward declarations
#include <intersector/intersector_fwd.hpp>

// LibSmartCar
#include <helpers/helpers.hpp>
#include <config/config_handler.hpp>

namespace App {

class Intersector {
public:
    Intersector(const std::string& intersect_shader_name);
    Intersector(const Config::IntersectorConfig& config);

    void ClearObstacles();
    void AddObstacles(const std::vector<MemoryAlignedBBox>& new_obstacle_bboxes);

    void ClearCarParts();
    void AddCarParts(const std::vector<MemoryAlignedBBox>& new_car_parts_bboxes);

    // TODO: should not be const, because returns intersection values
    bool Execute() const;

private:
    std::vector<MemoryAlignedBBox> obstacle_bboxes_;
    std::vector<MemoryAlignedBBox> car_parts_bboxes_;

    const std::string intersect_shader_name_;
};

} // namespace App