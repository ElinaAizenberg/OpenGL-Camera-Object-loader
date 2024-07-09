#include <iostream>
#include "../include/object.h"
#include "../include/loader.h"
#include "portable-file-dialogs.h"



void Object::loadObjectFile(const std::string& filepath) {
    rotation_[0] = 0;
    rotation_[1] = 0;
    rotation_[2] = 0;

    vertices_.clear();
    shapes_.clear();
    try{
        ObjectLoader::loadObFileData(filepath, vertices_, shapes_);
    }
    catch(...) {
        pfd::message("Problem", "Error: Unable to load file '" + filepath + "'. Please check if the file exists and you have the necessary permissions to read it.",
                     pfd::choice::ok, pfd::icon::error);
        return;
    }
    bounding_box_ = calculateBoundingBox();

    size_ = calculateObjectSize();
    max_dimension_ = glm::max(size_.x, size_.y);
}

void Object::draw()
{
    glRotatef(rotation_[0], 1,0,0);
    glRotatef(rotation_[1], 0,1,0);
    glRotatef(rotation_[2], 0,0,1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glColor3f(1, 1, 1);  // Set color to white
    for (auto const& shape : shapes_)
    {
        int num_ind = static_cast<int>(shape.size());

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, vertices_.data());

        glDrawElements(GL_TRIANGLES, num_ind, GL_UNSIGNED_INT, shape.data());

        glDisableClientState(GL_VERTEX_ARRAY);
    }

}

BoundingBox Object::calculateBoundingBox(){
    glm::vec3 min = {vertices_[0], vertices_[1],vertices_[2]};
    glm::vec3 max = {vertices_[0], vertices_[1],vertices_[2]};

    for (size_t i = 3; i < vertices_.size(); i += 3) {
        min[0] = std::min(vertices_[i], min[0]);
        min[1] = std::min(vertices_[i+1], min[1]);
        min[2] = std::min(vertices_[i+2], min[2]);

        max[0] = std::max(vertices_[i], max[0]);
        max[1] = std::max(vertices_[i+1], max[1]);
        max[2] = std::max(vertices_[i+2], max[2]);
    }

    return {min, max};
}

glm::vec3 Object::calculateObjectSize() const {
    return bounding_box_.max - bounding_box_.min;
}

float Object::calculateScalingFactor(float reference_size) const {
    return reference_size / max_dimension_;
}

void Object::rotateObjects(int i, int direction) {
    rotation_[i] += 90.0*direction;
    rotation_[i] = (rotation_[i] % 360 + 360) % 360;
}