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
#include <car_model/car_model.hpp>

namespace App {

class Intersector {
public:
    Intersector(const std::string& intersect_shader_name);
    Intersector(const Config::IntersectorConfig& config);

    void ClearObstacles();
    void AddObstacles(const Model* model);

    void ClearCarParts();
    void AddCarParts(const CarModel* car_model);

    void Intersect();

    std::vector<int> GetIntersectedObstacleMeshIndices(const int model_index) const {
        std::vector<int> intersected_obstacle_mesh_indices{};
        for (auto obstacle_index : results_.first) {
            auto result = obstacle_index_to_model_index_mesh_index.at(obstacle_index);
            if (result.first == model_index) {
                intersected_obstacle_mesh_indices.push_back(result.second);
            }
        }
        return intersected_obstacle_mesh_indices;
    }

    std::vector<int> GetIntersectedCarPartMeshIndices(const int model_index) const {
        std::vector<int> intersected_car_part_mesh_indices{};
        for (auto car_part_index : results_.second) {
            auto result = car_part_index_to_model_index_mesh_index.at(car_part_index);
            if (result.first == model_index) {
                intersected_car_part_mesh_indices.push_back(result.second);
            }
        }
        return intersected_car_part_mesh_indices;
    }

private:
    std::pair<int, int> ObstacleIndexToModelNameMeshIndex(const int obstacle_index) const {
        return obstacle_index_to_model_index_mesh_index.at(obstacle_index);
    }

    std::pair<int, int> CarPartIndexToModelNameMeshIndex(const int car_part_index) const {
        return car_part_index_to_model_index_mesh_index.at(car_part_index);
    }

    std::map<int, std::pair<int, int>> obstacle_index_to_model_index_mesh_index{};
    std::map<int, std::pair<int, int>> car_part_index_to_model_index_mesh_index{};
    int current_obstacle_index_ = 0;
    int current_car_part_index_ = 0;

    std::vector<MemoryAlignedBBox> obstacle_bboxes_;
    std::vector<MemoryAlignedBBox> car_parts_bboxes_;

    const std::string intersect_shader_name_;

    std::pair<std::vector<int>, std::vector<int>> results_;

    friend class Gui; // access to private variables
};

} // namespace App