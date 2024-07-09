
#include <tuple>
#include <iostream>
#include "../include/camera.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../include/config.h"


ViewCamera::ViewCamera(CameraMode mode, glm::vec3 camera_position, glm::vec3 target_position, glm::vec3 up_direction) :
mode_(mode), camera_position_(camera_position), target_position_(target_position), up_direction_(up_direction)
{
    initial_coordinates_ = {camera_position, target_position, up_direction};
}

void ViewCamera::applyMatrix() {
    glm::mat4 view_matrix =
            glm::lookAt(camera_position_, target_position_, up_direction_);
    GLfloat* matrix_data = glm::value_ptr(view_matrix);
    glMultMatrixf(matrix_data);
}

void ViewCamera::setView(float dim_ration) {
    if (view_ == kOrthogonal)
    {
        auto ortho_c = Config::getParameters().ortho_coefficient_;

        glOrtho(view_params_.left * ortho_c,
                view_params_.right * ortho_c,
                view_params_.bottom*dim_ration * ortho_c,
                view_params_.top*dim_ration * ortho_c,
                view_params_.near,
                view_params_.far);
    }
    else
    {
        glFrustum(view_params_.left,
                  view_params_.right,
                  view_params_.bottom*dim_ration,
                  view_params_.top*dim_ration,
                  view_params_.near,
                  view_params_.far);
    }

}

void ViewCamera::zoom(float zooming_factor) {
    view_params_.left += zooming_factor;
    view_params_.right -= zooming_factor;
    view_params_.top    = view_params_.top - zooming_factor;
    view_params_.bottom = view_params_.bottom + zooming_factor;
}

void ViewCamera::resetView() {
    view_params_.left = -1;
    view_params_.right = 1;
    view_params_.bottom = -1;
    view_params_.top = 1;
}

std::string ViewCamera::getCameraMode() {
    switch (mode_) {
        case kDome: return "Dome Camera";
        case kFirstPerson: return "First Person Camera";
        default: return "Unknown";
    }
}

std::string ViewCamera::getCameraView() {
    switch (view_) {
        case kOrthogonal: return "Orthogonal view";
        case kPerspective: return "Perspective view";
        default: return "Unknown";
    }
}

void ViewCamera::resetCamera() {
    camera_position_ = initial_coordinates_[0];
    target_position_ = initial_coordinates_[1];
    up_direction_ = initial_coordinates_[2];
}


FirstPersonCamera::FirstPersonCamera(glm::vec3 camera_position, glm::vec3 target_position, glm::vec3 up_direction) : ViewCamera(kFirstPerson, camera_position, target_position, up_direction) {

}

void FirstPersonCamera::move(float delta_x, float delta_z) {
    camera_position_.x += delta_x;
    target_position_.x += delta_x;

    camera_position_.z += delta_z;
    target_position_.z += delta_z;
}

void FirstPersonCamera::rotate(float delta_x, float delta_y)  {
    yaw_ += delta_x;
    pitch_ += delta_y;

    glm::vec3 axisy(0.0f, 1.0f, 0.0f);
    glm::vec3 axisx(1.0f, 0.0f, 0.0f);
    glm::vec3 p = glm::normalize(target_position_ - camera_position_);

    target_position_ = glm::vec3(glm::rotate(glm::mat4(1.0f), delta_x, axisy)* glm::vec4(p, 1));
    target_position_ = glm::vec3(glm::rotate(glm::mat4(1.0f), delta_y, axisx)* glm::vec4(target_position_, 1));
    target_position_ += camera_position_;

}

DomeCamera::DomeCamera(glm::vec3 camera_position, glm::vec3 target_position, glm::vec3 up_direction,  double radius): ViewCamera (kDome, camera_position, target_position, up_direction), radius_(radius){
    camera_position_.z = radius_;
}

void DomeCamera::changeElevation()
{
    camera_position_.y = sin(pitch_) * radius_;
    double new_r            = radius_ * cos(pitch_);
    camera_position_.x = new_r * cos(yaw_);
    camera_position_.z = new_r * sin(yaw_);
}

void DomeCamera::changeAzimuth()
{
    camera_position_.x = cos(yaw_) * radius_;
    camera_position_.z = sin(yaw_) * radius_;
}

void DomeCamera::rotate(float delta_x, float delta_y) {
    yaw_ += delta_x;
    pitch_ += delta_y;

    // Limit the pitch angle to avoid flipping
    if (pitch_ > glm::pi<float>() / 2.0f) {
        pitch_ = glm::pi<float>() / 2.0f;
    } else if (pitch_ < -glm::pi<float>() / 2.0f) {
        pitch_ = -glm::pi<float>() / 2.0f;
    }

    changeAzimuth();
    changeElevation();
}

void DomeCamera::viewOrtho(DomeCameraRotate direction) {
    if (direction == kFront){
        camera_position_ = glm::vec3(0.0f, 0.0f, 10.0);
        up_direction_ = glm::vec3(0.0f, 1.0f, 0.0f);
    }
    if (direction == kTop){
        camera_position_ = glm::vec3(0.0f, 10.0f, 0.0);
        up_direction_ = glm::vec3(0.0f, 0.0f, -1.0f);
    }
    if (direction == kSide){
        camera_position_ = glm::vec3(10.0f, 0.0f, 0.0);
        target_position_ = glm::vec3 (0,0,0);
        up_direction_ = glm::vec3(0.0f, 1.0f, 0.0f);
    }
    applyMatrix();
}

void DomeCamera::move(float delta_x, float delta_z) {
    auto ortho_c = Config::getParameters().ortho_coefficient_;
    view_params_.left += delta_x/ortho_c;
    view_params_.right += delta_x/ortho_c;

    view_params_.top    -= delta_z/ortho_c;
    view_params_.bottom -= delta_z/ortho_c;
}
