#pragma once

// OpenGL Wrapper
#include <GL/OOGL.hpp>
#include <GL/Math/Mat4.hpp>

// Constants
#include <constants/constants.hpp>

// Forward declarations
#include <transform/transform_fwd.hpp>

namespace App {

class Transform {
public:
    Transform();
    Transform(GL::Mat4 transform_matrix);

    void SetScale(GL::Vec3 scale);
    void UpdateScale(GL::Vec3 additional_scale);

    void SetRotation(float rotate_degrees, GL::Vec3 rotate_axis);
    void UpdateRotation(float additional_rotate_degrees, GL::Vec3 additional_rotate_axis);

    void SetTranslation(GL::Vec3 translation);
    void UpdateTranslation(GL::Vec3 additional_translation);

    operator const GL::Mat4() const;

private:
    void UpdateMatrix();

    GL::Mat4 scale_matrix_;
    GL::Mat4 rotate_matrix_;
    GL::Mat4 translation_matrix_;

    GL::Mat4 transform_matrix_;
};

} // namespace App