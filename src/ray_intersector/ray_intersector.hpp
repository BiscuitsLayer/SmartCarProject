#pragma once

#define NOMINMAX
#include <algorithm>
#include <limits>
#include <numbers>

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

struct Ray {
    GL::Vec4 origin;
    GL::Vec4 direction;
    GL::Mat4 car_model_matrix;
};

class RayIntersector {
public:
    RayIntersector(const std::string& intersect_shader_name)
    : intersect_shader_name_(intersect_shader_name) {
        distances.fill(std::numeric_limits<float>::infinity());
    }

    RayIntersector(const Config::IntersectorConfig& config)
    : RayIntersector(config.shader.compute_shader_name) {
        distances.fill(std::numeric_limits<float>::infinity());
    }

    void ClearObstacles() {
        obstacle_bboxes_.clear();
        current_obstacle_index_ = 0;
    }

    void AddObstacles(const Model* model) {
        std::vector<MemoryAlignedBBox> new_obstacle_bboxes = model->CollectMABB();
        obstacle_bboxes_.reserve(obstacle_bboxes_.size() + new_obstacle_bboxes.size());

        for (int bbox_index = 0; bbox_index < new_obstacle_bboxes.size(); ++bbox_index) {
            obstacle_bboxes_.push_back(new_obstacle_bboxes[bbox_index]);
            // obstacle_index_to_model_index_mesh_index[obstacle_bboxes_.size() - 1] = std::make_pair(current_obstacle_index_, bbox_index);
        }
        ++current_obstacle_index_;
    }

    void Intersect(GL::Mat4 car_model_matrix) {
        // TODO: get rid of magic numbers
        // TODO: fix code (with SubData) instead of generating buffer object every frame
        // TODO: fix SubData and GetSubData in OOGL (check if they need glBindBufferBase)

        std::array<Ray, APP_RAY_INTERSECTOR_RAYS_COUNT> rays_;
        //  = {
        //     // WARNING: origins have w component == 1.0, because they can be translated
        //     // but direction have w component == 0.0, because the should not be translated (only rotated)
        //     Ray{GL::Vec4{0.000, 0.000, 0.000, 1.000}, GL::Vec4{+1.000, 0.000, 0.000, 0.000}, car_model_matrix},
        //     Ray{GL::Vec4{0.000, 0.000, 0.000, 1.000}, GL::Vec4{+0.866, 0.000, 0.500, 0.000}, car_model_matrix},
        //     Ray{GL::Vec4{0.000, 0.000, 0.000, 1.000}, GL::Vec4{+0.500, 0.000, 0.866, 0.000}, car_model_matrix},
        //     Ray{GL::Vec4{0.000, 0.000, 0.000, 1.000}, GL::Vec4{+0.000, 0.000, 1.000, 0.000}, car_model_matrix},
        //     Ray{GL::Vec4{0.000, 0.000, 0.000, 1.000}, GL::Vec4{-0.500, 0.000, 0.866, 0.000}, car_model_matrix},
        //     Ray{GL::Vec4{0.000, 0.000, 0.000, 1.000}, GL::Vec4{-0.866, 0.000, 0.500, 0.000}, car_model_matrix},
        //     Ray{GL::Vec4{0.000, 0.000, 0.000, 1.000}, GL::Vec4{-1.000, 0.000, 0.000, 0.000}, car_model_matrix}
        // };

        assert(APP_RAY_INTERSECTOR_RAYS_COUNT > 1);
        float coef = std::numbers::pi / (APP_RAY_INTERSECTOR_RAYS_COUNT - 1);
        for (int k = 0; k < APP_RAY_INTERSECTOR_RAYS_COUNT; ++k) {
            float x_component = std::cos(coef * k);
            float z_component = std::sin(coef * k);

            rays_[k] = Ray{GL::Vec4{0.000, 0.000, 0.000, 1.000}, GL::Vec4{x_component, 0.000, z_component, 0.000}, car_model_matrix};
        }

        auto& context = App::Context::Get();
        auto& gl = context.gl->get();
        auto shader_handler = context.shader_handler.value();

        auto intersect_program = shader_handler.at(intersect_shader_name_);
        gl.UseProgram(*intersect_program);

        auto obstacle_ssbo = GL::StorageBuffer(obstacle_bboxes_.data(), obstacle_bboxes_.size() * sizeof(MemoryAlignedBBox), GL::BufferUsage::StaticRead, 0);
        auto car_parts_ssbo = GL::StorageBuffer(rays_.data(), rays_.size() * sizeof(Ray), GL::BufferUsage::StaticRead, 1);

        std::vector<float> intersection_results(obstacle_bboxes_.size() * rays_.size(), -1.0);
        auto intersection_result_ssbo = GL::StorageBuffer(intersection_results.data(), intersection_results.size() * sizeof(float), GL::BufferUsage::DynamicDraw, 2);

        // Execute compute shader
        gl.DispatchCompute(obstacle_bboxes_.size(), rays_.size(), APP_INTERSECTOR_NUM_GROUPS_Z_COUNT);
        gl.Barrier(GL::BarrierBit::All);

        // Get the results back on CPU
        auto read_data = intersection_result_ssbo.Map<float>(GL::BufferAccess::ReadOnly);
        distances.fill(std::numeric_limits<float>::infinity());

        for (int i = 0; i < intersection_results.size(); ++i) {
            if (read_data[i] > 0) {
                // i = obstacle_id * rays_size + ray_id
                int obstacle_id = i / rays_.size();
                int ray_id = i % rays_.size();

                distances[ray_id] = std::min<float>(distances[ray_id], read_data[i]);
            }
        }
    }

    std::array<float, APP_RAY_INTERSECTOR_RAYS_COUNT> GetResultDistances() const {
        return distances;
    }

private:
    int current_obstacle_index_ = 0;

    std::vector<MemoryAlignedBBox> obstacle_bboxes_;

    const std::string intersect_shader_name_;

    std::array<float, APP_RAY_INTERSECTOR_RAYS_COUNT> distances;

    friend class Gui; // access to private variables
};

} // namespace App