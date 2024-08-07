#ifndef PROJECT_2_OBJECT_H
#define PROJECT_2_OBJECT_H


#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>



class Object{
public:
    Object() = default;

    void loadObjectFile(const std::string& filepath);
    void draw();
    float calculateScalingFactor(float reference_size) const;
    void rotateObjects(int i, int direction);

private:
    struct BoundingBox {
        glm::vec3 min{0.0, 0.0, 0.0};
        glm::vec3 max{0.0, 0.0, 0.0};
    };

    std::vector<GLfloat> vertices_{};
    std::vector<std::vector<unsigned int>> shapes_;

    float max_length_{0.0};
    int rotation_[3] = {0,0,0};

    BoundingBox calculateBoundingBox();
    static float calculateObjectSize(Object::BoundingBox bounding_box) ;

};

#endif //PROJECT_2_OBJECT_H
