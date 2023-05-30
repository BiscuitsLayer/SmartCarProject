#include "car_model.hpp"

namespace App {

// Extern variables
extern const GL::Vec3 APP_CAR_WHEELS_ROTATION_AXIS;

CarModel::CarModel(const std::string& name, const std::string& default_shader_name,
    const std::string& bbox_shader_name, const std::string& gltf,
    const std::vector<std::string>& wheel_meshes_names, const float move_max_speed,
    const float acceleration, const float rotate_max_speed, const float wheels_rotate_speed,
    const GL::Vec3& center_translation, const Transform& transform)
    : Model(name, default_shader_name, bbox_shader_name, gltf, transform),
    move_max_speed_(move_max_speed),
    rotate_max_speed_(rotate_max_speed),
    wheels_rotate_speed_(wheels_rotate_speed),
    center_translation_(GL::Mat4{}.Translate(center_translation)),
    accelerator_(Accelerator{move_max_speed, acceleration}) {
    wheel_meshes_indicies_.reserve(wheel_meshes_names.size());
    for (auto&& wheel_mesh_name : wheel_meshes_names) {
        auto found = std::find_if(meshes_.begin(), meshes_.end(), [wheel_mesh_name](const App::Mesh& mesh) {
            return mesh.name_ == wheel_mesh_name;
        });
        if (found != meshes_.end()) {
            wheel_meshes_indicies_.push_back(found - meshes_.begin());
        }
    }
}

CarModel::CarModel(const Config::CarModelConfig& config)
    : CarModel(config.name, config.shader.default_shader_name, config.shader.bbox_shader_name,
        config.gltf, config.wheels.mesh_names, config.speed.move, config.acceleration, config.speed.rotate,
        config.wheels.speed.rotate, config.rotation_center, config.transform) {}

const GL::Mat4 CarModel::GetModelMatrix() const {
    return static_cast<GL::Mat4>(transform_) * movement_transform_ * center_translation_;
}

void CarModel::Move(float delta_time) {
    auto& context = App::Context::Get();
    if (context.keyboard_mode.value() == App::KeyboardMode::CAR_MOVEMENT) {
        if (context.keyboard_status.value()[GL::Key::W]) {
            accelerator_.IncreaseSpeed(delta_time, true);
        }
        if (context.keyboard_status.value()[GL::Key::S]) {
            accelerator_.IncreaseSpeed(delta_time, false);
        }
        if (context.keyboard_status.value()[GL::Key::A]) {
            RotateLeft(delta_time, accelerator_.GetSpeed() > 0.0);
        }
        if (context.keyboard_status.value()[GL::Key::D]) {
            RotateRight(delta_time, accelerator_.GetSpeed() > 0.0);
        }
    } else if (context.keyboard_mode.value() == App::KeyboardMode::NN_LEARNING) {
        if (context.actions[0]) {
            accelerator_.IncreaseSpeed(delta_time, true);
        }
        if (context.actions[1]) {
            accelerator_.IncreaseSpeed(delta_time, false);
        }
        if (context.actions[2]) {
            RotateLeft(delta_time, accelerator_.GetSpeed() > 0.0);
        }
        if (context.actions[3]) {
            RotateRight(delta_time, accelerator_.GetSpeed() > 0.0);
        }
    }
    MoveForward(delta_time);

    // Do intersection
    std::vector<int> intersection_result{};
    if (collision_intersector_) {
        collision_intersector_->ClearCarParts();
        collision_intersector_->AddCarParts(this);

        collision_intersector_->Intersect();
        intersection_result = collision_intersector_->GetIntersectedCarPartMeshIndices(0);
    }
    
    // No collisions found - car can be moved
    if (intersection_result.empty()) { 
        UpdateMovementTransform();
        RotateWheels(accelerator_.GetSpeed() * delta_time);
    } else {
        accelerator_.Stop();
        for (auto mesh_index : intersection_result) {
            DrawBBoxOnCollision(mesh_index);
        }
    }
    ClearPrecomputedMovementTransform();

    // If the car is moving - keep updating the camera
    // If the car is still - update the camera only in case there was a collision
    if ((accelerator_.GetSpeed() != 0.0f) || (accelerator_.WasStopped() && !context.camera->ReachedFinalPosition())) {
        context.camera->UpdateWithModel(GetModelMatrix());
    }

    if (!context.keyboard_status.value()[GL::Key::W] && !context.keyboard_status.value()[GL::Key::S]) {
        accelerator_.DecreaseSpeed(delta_time);
    }

    // Update distances to obstacles
    ray_intersector_->Intersect(GetModelMatrix());
    context.distances_from_rays = ray_intersector_->GetResultDistances();
}

void CarModel::SetDrawWheelsBBoxes(bool value) {
    for (auto&& index : wheel_meshes_indicies_) {
        meshes_[index].SetDrawBBox(value);
    }
}

std::vector<MemoryAlignedBBox> CarModel::CollectMABB() const {
    std::vector<MemoryAlignedBBox> result;
    result.reserve(meshes_.size());

    for (auto&& mesh : meshes_) {
        auto mabb = mesh.GetMABB();
        mabb.model = GetPrecomputedModelMatrix();
        result.push_back(mabb);
    }
    return result;
}

void CarModel::RotateWheels(float delta_time) {
    for (auto index : wheel_meshes_indicies_) {
        meshes_[index].self_transform_.UpdateRotation(wheels_rotate_speed_ * delta_time, APP_CAR_WHEELS_ROTATION_AXIS);
        meshes_[index].bbox_.UpdateVertices(meshes_[index].self_transform_);
    }
}

void CarModel::MoveForward(float delta_time) {
    precomputed_movement_transform_.Translate(GL::Vec3(0.0f, 0.0f, accelerator_.GetSpeed() * delta_time));
}

void CarModel::RotateLeft(float delta_time, bool move_front) {
    if (accelerator_.GetSpeed() != 0.0) {
        float rotate_speed = rotate_max_speed_ * accelerator_.GetSpeed() / move_max_speed_;
        precomputed_movement_transform_.Rotate(GL::Vec3(0.0f, 1.0f, 0.0f), rotate_speed * delta_time);
    }
}

void CarModel::RotateRight(float delta_time, bool move_front) {
    if (accelerator_.GetSpeed() != 0.0) {
        float rotate_speed = rotate_max_speed_ * accelerator_.GetSpeed() / move_max_speed_;
        precomputed_movement_transform_.Rotate(GL::Vec3(0.0f, 1.0f, 0.0f), -1.0f * rotate_speed * delta_time);
    }
}

void CarModel::ClearPrecomputedMovementTransform() {
    precomputed_movement_transform_ = movement_transform_;
}

void CarModel::UpdateMovementTransform() {
    movement_transform_ = precomputed_movement_transform_;
}

const GL::Mat4 CarModel::GetPrecomputedModelMatrix() const {
    return static_cast<GL::Mat4>(transform_) * precomputed_movement_transform_ * center_translation_;
}

} // namespace App