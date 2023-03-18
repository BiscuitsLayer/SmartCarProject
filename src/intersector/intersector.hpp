#pragma once

// OpenGL Wrapper
#include <GL/OOGL.hpp>

namespace App {

class Intersector {
public:
    Intersector(std::string intersect_shader_name,
    std::vector<std::pair<GL::Vec3, GL::Vec3>> obstacle_minmaxes,
    std::vector<std::pair<GL::Vec3, GL::Vec3>> car_parts_minmaxes)
    : intersect_shader_name_(intersect_shader_name) {}

    void Execute() {
        auto& context = App::Context::Get();
        auto& gl = context.gl.value().get();
        auto intersect_program = shader_handler.at(intersect_shader_name);
        gl.UseProgram(*intersect_program);
        
        auto obstacle_ssbo = GL::StorageBuffer(obstacle_bboxes.data(), obstacle_bboxes.size() * sizeof(CS_BBox), GL::BufferUsage::StaticRead, 0);
        auto car_parts_ssbo = GL::StorageBuffer(car_parts_bboxes.data(), car_parts_bboxes.size() * sizeof(CS_BBox), GL::BufferUsage::StaticRead, 1);

        std::vector<float> intersection_results(obstacle_bboxes.size() * car_parts_bboxes.size(), 0);
        auto intersection_result_ssbo = GL::StorageBuffer(intersection_results.data(), intersection_results.size() * sizeof(float), GL::BufferUsage::DynamicDraw, 2);

        gl.UseProgram(compute_program);
        gl.DispatchCompute(obstacle_bboxes.size(), car_parts_bboxes.size(), 1);
        gl.Barrier(GL::BarrierBit::All);

        // TODO: change output format
        GLfloat* read_data = (GLfloat*)gl.MapStorageBuffer(GL::BufferAccess::ReadOnly);

        for (int i = 0; i < intersection_results.size(); ++i) {
            if (i % car_parts_bboxes.size() == 0) {
                std::cout << std::endl;
            }
            std::cout << read_data[i] << ' ';
        }
    }

private:
    struct MemoryAlignedBBox {
        MemoryAlignedBBox(GL::Vec3 new_min, GL::Vec3 new_max)
            : min(new_min), max(new_max) {}

        GL::Vec3 min;
        float alignment1 = 0.0f;
        GL::Vec3 max;
        float alignment2 = 0.0f;

        // TODO: add mesh transform there
    };

    std::vector<MemoryAlignedBBox> obstacle_bboxes_;
    std::vector<MemoryAlignedBBox> car_parts_bboxes_;

    std::string intersect_shader_name_;
};

} // namespace App