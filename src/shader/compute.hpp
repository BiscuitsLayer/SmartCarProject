#pragma once

// STL
#include <iostream>
#include <vector>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Sources
#include <helpers/helpers.hpp>
#include <shader/compute.hpp>

namespace App {

struct CS_BBox {
public:
    CS_BBox(GL::Vec3 new_min, GL::Vec3 new_max)
        : min(new_min), max(new_max) {}

    GL::Vec3 min;
    float alignment1 = 0.0f;
    GL::Vec3 max;
    float alignment2 = 0.0f;
};

struct ComputeShader {
    void Run() {
        auto& context = App::Context::Get();
        auto& gl = context.gl.value().get();

        GL::Shader compute_shader(GL::ShaderType::Compute, App::ReadFileData("../src/shader/compute.comp", false));
        GL::Program compute_program{compute_shader};

        std::vector<CS_BBox> obstacle_bboxes {
            CS_BBox{GL::Vec3{0.0, 0.0, 0.0}, GL::Vec3{10.0, 10.0, 10.0}},
                CS_BBox{ GL::Vec3{10.0, 10.0, 10.0}, GL::Vec3{20.0, 20.0, 20.0} },
                CS_BBox{ GL::Vec3{20.0, 20.0, 20.0}, GL::Vec3{30.0, 30.0, 30.0} }
        };

        std::vector<CS_BBox> car_parts_bboxes {
            CS_BBox{GL::Vec3{2.0, 2.0, 2.0}, GL::Vec3{4.0, 4.0, 4.0}},
                CS_BBox{ GL::Vec3{5.0, 5.0, 5.0}, GL::Vec3{8.0, 8.0, 8.0} },
                CS_BBox{ GL::Vec3{12.0, 12.0, 12.0}, GL::Vec3{14.0, 14.0, 14.0} },
                CS_BBox{ GL::Vec3{15.0, 15.0, 15.0}, GL::Vec3{18.0, 18.0, 18.0} },
                CS_BBox{ GL::Vec3{22.0, 22.0, 22.0}, GL::Vec3{24.0, 24.0, 24.0} },
                CS_BBox{ GL::Vec3{25.0, 25.0, 25.0}, GL::Vec3{28.0, 28.0, 28.0} }
        };

        auto obstacle_ssbo = GL::StorageBuffer(obstacle_bboxes.data(), obstacle_bboxes.size() * sizeof(CS_BBox), GL::BufferUsage::StaticRead, 0);
        auto car_parts_ssbo = GL::StorageBuffer(car_parts_bboxes.data(), car_parts_bboxes.size() * sizeof(CS_BBox), GL::BufferUsage::StaticRead, 1);

        std::vector<float> intersection_results(obstacle_bboxes.size() * car_parts_bboxes.size(), 0);
        auto intersection_result_ssbo = GL::StorageBuffer(intersection_results.data(), intersection_results.size() * sizeof(float), GL::BufferUsage::DynamicDraw, 2);

        gl.UseProgram(compute_program);
        gl.DispatchCompute(obstacle_bboxes.size(), car_parts_bboxes.size(), 1);
        gl.Barrier(GL::BarrierBit::All);

        GLfloat* read_data = (GLfloat*)gl.MapStorageBuffer(GL::BufferAccess::ReadOnly);

        for (int i = 0; i < intersection_results.size(); ++i) {
            if (i % car_parts_bboxes.size() == 0) {
                std::cout << std::endl;
            }
            std::cout << read_data[i] << ' ';
        }
    }

    bool Intersect(CS_BBox lhs, CS_BBox rhs) {
        return (lhs.min.X <= rhs.max.X) && (lhs.max.X >= rhs.min.X)
            && (lhs.min.Y <= rhs.max.Y) && (lhs.max.Y >= rhs.min.Y)
            && (lhs.min.Z <= rhs.max.Z) && (lhs.max.Z >= rhs.min.Z);
    }
};

} // namespace App