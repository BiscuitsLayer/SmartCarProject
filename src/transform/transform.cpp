#include "transform.hpp"

namespace App {

Transform::Transform()
    : transform_matrix_(GL::Mat4{}) {}

Transform::Transform(GL::Mat4 transform_matrix)
    : transform_matrix_(transform_matrix) {}

void Transform::SetScale(GL::Vec3 scale) {
    GL::Mat4 new_scale;
    new_scale.Scale(scale);

    scale_matrix_ = new_scale;
    UpdateMatrix();
}

void Transform::UpdateScale(GL::Vec3 additional_scale) {
    GL::Mat4 new_scale;
    new_scale.Scale(additional_scale);

    scale_matrix_ = new_scale * scale_matrix_;
    UpdateMatrix();
}

void Transform::SetRotation(float rotate_degrees, GL::Vec3 rotate_axis) {
    GL::Mat4 new_rotate;
    new_rotate.Rotate(rotate_axis, GL::Rad(rotate_degrees));

    rotate_matrix_ = new_rotate;
    UpdateMatrix();
}

void Transform::UpdateRotation(float additional_rotate_degrees, GL::Vec3 additional_rotate_axis) {
    GL::Mat4 new_rotate;
    new_rotate.Rotate(additional_rotate_axis, GL::Rad(additional_rotate_degrees));

    rotate_matrix_ = new_rotate * rotate_matrix_;
    UpdateMatrix();
}

void Transform::SetTranslation(GL::Vec3 translation) {
    GL::Mat4 new_translate;
    new_translate.Translate(translation);

    translation_matrix_ = new_translate;
    UpdateMatrix();
}

void Transform::UpdateTranslation(GL::Vec3 additional_translation) {
    GL::Mat4 new_translate;
    new_translate.Translate(additional_translation);

    translation_matrix_ = new_translate * translation_matrix_;
    UpdateMatrix();
}

Transform::operator GL::Mat4() {
    return transform_matrix_;
}

void Transform::UpdateMatrix() {
    transform_matrix_ = GL::Mat4{};
    transform_matrix_ = scale_matrix_ * transform_matrix_;
    transform_matrix_ = rotate_matrix_ * transform_matrix_;
    transform_matrix_ = translation_matrix_ * transform_matrix_;
}

GL::Mat4 GetModelViewProjectionMatrix(GL::Mat4 model, GL::Mat4 view, GL::Mat4 projection) {
    return projection * view * model;
}

GL::Mat4 GetViewNoTranslationProjectionMatrix(GL::Mat4 view, GL::Mat4 projection) {
    // remove translation from the view matrix
    view = GL::Mat4(
        view.m[0], view.m[4], view.m[8], 0,
        view.m[1], view.m[5], view.m[9], 0,
        view.m[2], view.m[6], view.m[10], 0,
        view.m[3], view.m[7], view.m[11], view.m[15]
    );

    return projection * view;
}

} // namespace App