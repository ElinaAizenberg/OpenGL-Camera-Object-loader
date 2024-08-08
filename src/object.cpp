#include "../include/object.h"
#include "../include/loader.h"
#include "portable-file-dialogs.h"


void Object::loadObjectFile(const std::string& filepath)
/**Loads vertices and indices from an .obj file using Loader class, calculates Object's bounding box and its diagonal length.
If the loading fails, an error message is displayed.*/
{
    rotation_[0] = 0;
    rotation_[1] = 0;
    rotation_[2] = 0;

    vertices_.clear();
    shapes_.clear();
    try
    {
        ObjectLoader::loadObFileData(filepath, vertices_, shapes_);
    }
    catch(...)
    {
        pfd::message("Problem", "Error: Unable to load file '" + filepath + "'. Please check if the file exists and you have the necessary permissions to read it.",
                     pfd::choice::ok, pfd::icon::error);
        return;
    }
    BoundingBox bounding_box = calculateBoundingBox();
    max_length_ = calculateObjectSize(bounding_box);
}

void Object::draw()
/** Renders an Object using OpenGL. */
{
    // glPolygonMode sets the polygon drawing mode, determining how polygons will be rasterized.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Apply rotation along each axis.
    glRotatef(float(rotation_[0]), 1,0,0);
    glRotatef(float(rotation_[1]), 0,1,0);
    glRotatef(float(rotation_[2]), 0,0,1);

    // Set color to white
    glColor3f(1, 1, 1);

    // Iterate though loaded shapes, where each shape contains a vector of indices.
    for (auto const& shape : shapes_)
    {
        int num_ind = static_cast<int>(shape.size());

        // Enables OpenGL to use the array of vertices specified later.
        glEnableClientState(GL_VERTEX_ARRAY);
        // glVertexPointer specifies the location and data format of an array of vertex coordinates to use when rendering
        glVertexPointer(3, GL_FLOAT, 0, vertices_.data());
        // Renders primitives from array data.
        // Due to mode GL_TRIANGLES it draws triangles using the indices stored in a shape.
        glDrawElements(GL_TRIANGLES, num_ind, GL_UNSIGNED_INT, shape.data());
        glDisableClientState(GL_VERTEX_ARRAY);
    }
}

Object::BoundingBox Object::calculateBoundingBox()
/** Calculates the bounding box of the Object based on its vertices.
Iterates through all the vertices of the object to determine the minimum and maximum x and y coordinates,
then adjusts them according to the current zoom factor from the individual ImGui window parameters.*/
{
    glm::vec3 min = {vertices_[0], vertices_[1],vertices_[2]};
    glm::vec3 max = {vertices_[0], vertices_[1],vertices_[2]};

    for (size_t i = 3; i < vertices_.size(); i += 3)
    {
        min[0] = std::min(vertices_[i], min[0]);
        min[1] = std::min(vertices_[i+1], min[1]);
        min[2] = std::min(vertices_[i+2], min[2]);

        max[0] = std::max(vertices_[i], max[0]);
        max[1] = std::max(vertices_[i+1], max[1]);
        max[2] = std::max(vertices_[i+2], max[2]);
    }

    return {min, max};
}

float Object::calculateObjectSize(Object::BoundingBox bounding_box)
/** Calculates the diagonal length of the bounding box, which represents the object's size.*/
{
    glm::vec3 vec_diff = bounding_box.max - bounding_box.min;
    return std::sqrt(vec_diff.x * vec_diff.x + vec_diff.y * vec_diff.y + vec_diff.z * vec_diff.z);
}

float Object::calculateScalingFactor(float reference_size) const
/** Computes the scaling factor based on a given reference size relative to the object's
diagonal length of the bounding box.*/
{
    return reference_size / max_length_;
}

void Object::rotateObjects(int i, int direction)
/** Rotates the object by 90 degrees in the specified direction defined by index,
ensuring the rotation angle stays within 0-359 degrees.*/
{
    rotation_[i] += 90 * direction;
    rotation_[i] = (rotation_[i] % 360 + 360) % 360;
}