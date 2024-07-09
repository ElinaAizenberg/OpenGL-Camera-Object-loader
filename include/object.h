#ifndef PROJECT_2_OBJECT_H
#define PROJECT_2_OBJECT_H


#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;
};


class Object{
public:
    Object() = default;

    void loadObjectFile(const std::string& filepath);
    void draw();
    float calculateScalingFactor(float reference_size) const;
    void rotateObjects(int i, int direction);

private:

    BoundingBox bounding_box_;
    BoundingBox calculateBoundingBox();

    glm::vec3 calculateObjectSize() const;


    std::vector<float> text_vertices_;
    std::vector<std::vector<unsigned int>> shapes_;

    std::vector<GLfloat> vertices_{};

    glm::vec3 size_;
    float max_dimension_;

    int rotation_[3] = {0,0,0};

};

#endif //PROJECT_2_OBJECT_H
