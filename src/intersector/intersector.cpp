#include "intersector.hpp"

namespace App {

// Extern variables
extern const int APP_INTERSECTOR_NUM_GROUPS_Z_COUNT;
extern const float APP_INTERSECTOR_INTERSECTION_FOUND;
extern const float APP_INTERSECTOR_INTERSECTION_NOT_FOUND;

Intersector::Intersector(const std::string& intersect_shader_name)
    : intersect_shader_name_(intersect_shader_name) {}

Intersector::Intersector(const Config::IntersectorConfig& config)
    : Intersector(config.shader.compute_shader_name) {}

void Intersector::ClearObstacles() {
    obstacle_bboxes_.clear();
    current_obstacle_index_ = 0;
}

void Intersector::AddObstacles(const Model* model) {
    std::vector<MemoryAlignedBBox> new_obstacle_bboxes = model->CollectMABB();
    obstacle_bboxes_.reserve(obstacle_bboxes_.size() + new_obstacle_bboxes.size());

    for (int bbox_index = 0; bbox_index < new_obstacle_bboxes.size(); ++bbox_index) {
        obstacle_bboxes_.push_back(new_obstacle_bboxes[bbox_index]);
        obstacle_index_to_model_index_mesh_index[obstacle_bboxes_.size() - 1] = std::make_pair(current_obstacle_index_, bbox_index);
    }
    ++current_obstacle_index_;
}

void Intersector::ClearCarParts() {
    car_parts_bboxes_.clear();
    current_car_part_index_ = 0;
}

void Intersector::AddCarParts(const CarModel* car_model) {
    std::vector<MemoryAlignedBBox> new_car_parts_bboxes = car_model->CollectMABB();
    car_parts_bboxes_.reserve(car_parts_bboxes_.size() + new_car_parts_bboxes.size());

    for (int bbox_index = 0; bbox_index < new_car_parts_bboxes.size(); ++bbox_index) {
        car_parts_bboxes_.push_back(new_car_parts_bboxes[bbox_index]);
        car_part_index_to_model_index_mesh_index[car_parts_bboxes_.size() - 1] = std::make_pair(current_car_part_index_, bbox_index);
    }
    ++current_car_part_index_;
}

void Intersector::Intersect() {
    // TODO: get rid of magic numbers
    // TODO: fix code (with SubData) instead of generating buffer object every frame
    // TODO: fix SubData and GetSubData in OOGL (check if they need glBindBufferBase)
    std::vector<int> obstacles_collided_ids{};
    std::vector<int> car_parts_collided_ids{};

    auto& context = App::Context::Get();
    auto& gl = context.gl->get();
    auto shader_handler = context.shader_handler.value();

    auto intersect_program = shader_handler.at(intersect_shader_name_);
    gl.UseProgram(*intersect_program);

    auto obstacle_ssbo = GL::StorageBuffer(obstacle_bboxes_.data(), obstacle_bboxes_.size() * sizeof(MemoryAlignedBBox), GL::BufferUsage::StaticRead, 0);
    auto car_parts_ssbo = GL::StorageBuffer(car_parts_bboxes_.data(), car_parts_bboxes_.size() * sizeof(MemoryAlignedBBox), GL::BufferUsage::StaticRead, 1);

    std::vector<float> intersection_results(obstacle_bboxes_.size() * car_parts_bboxes_.size(), APP_INTERSECTOR_INTERSECTION_NOT_FOUND);
    auto intersection_result_ssbo = GL::StorageBuffer(intersection_results.data(), intersection_results.size() * sizeof(float), GL::BufferUsage::DynamicDraw, 2);

    // Execute compute shader
    gl.DispatchCompute(obstacle_bboxes_.size(), car_parts_bboxes_.size(), APP_INTERSECTOR_NUM_GROUPS_Z_COUNT);
    gl.Barrier(GL::BarrierBit::All);

    // Get the results back on CPU
    auto read_data = intersection_result_ssbo.Map<float>(GL::BufferAccess::ReadOnly);

    float sum = 0;
    for (int i = 0; i < intersection_results.size(); ++i) {
        if (read_data[i] == APP_INTERSECTOR_INTERSECTION_FOUND) {
            // i = obstacle_id * car_parts_size + car_parts_id
            int obstacle_id = i / car_parts_bboxes_.size();
            obstacles_collided_ids.push_back(obstacle_id);

            int car_parts_id = i % car_parts_bboxes_.size();
            car_parts_collided_ids.push_back(car_parts_id);
        }
    }
    results_ = std::make_pair(obstacles_collided_ids, car_parts_collided_ids);
}

} // namespace App