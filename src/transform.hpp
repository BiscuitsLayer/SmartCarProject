#pragma once

// OpenGL Wrapper
#include <GL/Math/Mat4.hpp>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace App {

class Transform {
public:
    Transform(): transform_matrix_(glm::mat4(1.0f)) {
    }

    void SetScale(glm::vec3 scale) {
        scale_ = scale;
        UpdateMatrix();
    }

    void UpdateScale(glm::vec3 additional_scale) {
        scale_ *= additional_scale;
        UpdateMatrix();
    }

    void SetRotation(float rotate_degrees_x, float rotate_degrees_y, float rotate_degrees_z) {
        rotate_degrees_x_ = rotate_degrees_x;
        rotate_degrees_y_ = rotate_degrees_y;
        rotate_degrees_z_ = rotate_degrees_z;
        UpdateMatrix();
    }

    void UpdateRotation(float additional_rotate_degrees_x, float additional_rotate_degrees_y, float additional_rotate_degrees_z) {
        rotate_degrees_x_ += additional_rotate_degrees_x;
        rotate_degrees_y_ += additional_rotate_degrees_y;
        rotate_degrees_z_ += additional_rotate_degrees_z;
        UpdateMatrix();
    }

    void SetTranslation(glm::vec3 translation) {
        translation_ = translation;
        UpdateMatrix();
    }

    void UpdateTranslation(glm::vec3 additional_translation) {
        translation_ += additional_translation;
        UpdateMatrix();
    }

    glm::vec4 Apply(const glm::vec4 &vector) const {
        return transform_matrix_ * vector;
    }

    GL::Mat4 GetMat4() const {
        const float* data_ptr = glm::value_ptr(transform_matrix_);
        GL::Mat4 result {
            data_ptr[0],  data_ptr[1],  data_ptr[2],  data_ptr[3],  
            data_ptr[4],  data_ptr[5],  data_ptr[6],  data_ptr[7],  
            data_ptr[8],  data_ptr[9],  data_ptr[10], data_ptr[11],  
            data_ptr[12], data_ptr[13], data_ptr[14], data_ptr[15],  
        };
        return result;
    }

private:
    void UpdateMatrix() {
        transform_matrix_ = glm::mat4(1.0f);
        transform_matrix_ = glm::scale(transform_matrix_, scale_);

        transform_matrix_ = glm::rotate(transform_matrix_, glm::radians(rotate_degrees_x_), glm::vec3(1.0f, 0.0f, 0.0f));
        transform_matrix_ = glm::rotate(transform_matrix_, glm::radians(rotate_degrees_y_), glm::vec3(0.0f, 1.0f, 0.0f));
        transform_matrix_ = glm::rotate(transform_matrix_, glm::radians(rotate_degrees_z_), glm::vec3(0.0f, 0.0f, 1.0f));

        transform_matrix_ = glm::translate(transform_matrix_, translation_);
    }

    // scale
    glm::vec3 scale_ = glm::vec3(1.0f);

    // rotate
    float rotate_degrees_x_ = 0.0f;
    float rotate_degrees_y_ = 0.0f;
    float rotate_degrees_z_ = 0.0f;

    // translation
    glm::vec3 translation_ = glm::vec3(0.0f);

    // result matrix to apply
    glm::mat4 transform_matrix_ = glm::mat4(1.0f);
};

}