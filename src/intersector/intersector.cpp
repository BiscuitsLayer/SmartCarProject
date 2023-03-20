#include "intersector.hpp"

namespace App {

Intersector::Intersector(std::string intersect_shader_name)
    : intersect_shader_name_(intersect_shader_name) {}

Intersector::Intersector(Config::IntersectorConfig config)
    : Intersector(config.shader.compute_shader_name) {}

void Intersector::ClearObstacles() {
    obstacle_bboxes_.clear();
}

void Intersector::AddObstacles(std::vector<MemoryAlignedBBox> new_obstacle_bboxes) {
    obstacle_bboxes_.reserve(obstacle_bboxes_.size() + new_obstacle_bboxes.size());
    for (auto&& bbox : new_obstacle_bboxes) {
        obstacle_bboxes_.push_back(bbox);
    }
}

void Intersector::ClearCarParts() {
    car_parts_bboxes_.clear();
}

void Intersector::AddCarParts(std::vector<MemoryAlignedBBox> new_car_parts_bboxes) {
    car_parts_bboxes_.reserve(car_parts_bboxes_.size() + new_car_parts_bboxes.size());
    for (auto&& bbox : new_car_parts_bboxes) {
        car_parts_bboxes_.emplace_back(bbox);
    }
}

void Intersector::Execute() {
    // TODO: get rid of magic numbers

    auto& context = App::Context::Get();
    auto& gl = context.gl.value().get();
    auto shader_handler = context.shader_handler.value();

    auto intersect_program = shader_handler.at(intersect_shader_name_);
    gl.UseProgram(*intersect_program);

    auto obstacle_ssbo = GL::StorageBuffer(obstacle_bboxes_.data(), obstacle_bboxes_.size() * sizeof(MemoryAlignedBBox), GL::BufferUsage::StaticRead, 0);
    auto car_parts_ssbo = GL::StorageBuffer(car_parts_bboxes_.data(), car_parts_bboxes_.size() * sizeof(MemoryAlignedBBox), GL::BufferUsage::StaticRead, 1);

    std::vector<float> intersection_results(obstacle_bboxes_.size() * car_parts_bboxes_.size(), 0);
    auto intersection_result_ssbo = GL::StorageBuffer(intersection_results.data(), intersection_results.size() * sizeof(float), GL::BufferUsage::DynamicDraw, 2);

    gl.DispatchCompute(obstacle_bboxes_.size(), car_parts_bboxes_.size(), 1);
    gl.Barrier(GL::BarrierBit::All);

    auto read_data = intersection_result_ssbo.Map<float>(GL::BufferAccess::ReadOnly);

    float sum = 0;
    for (int i = 0; i < intersection_results.size(); ++i) {
        // if (i % car_parts_bboxes.size() == 0) {
        //     std::cout << std::endl;
        // }
        // std::cout << (*read_data)[i] << ' ';
        sum += *(read_data + i);
    }

    // std::cout << "sum = " << sum << std::endl;
}

} // namespace App