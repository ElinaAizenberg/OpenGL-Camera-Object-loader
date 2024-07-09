#ifndef PROJECT_2_CAMERA_H
#define PROJECT_2_CAMERA_H

#include <glm/glm.hpp>
#include <vector>


enum View
{
    kOrthogonal,
    kPerspective
};

enum CameraMode
{
    kFirstPerson,
    kDome
};

enum DomeCameraRotate{
    kFront,
    kSide,
    kTop,
    kFree
};

class ViewCamera{
public:
    struct ViewParams
    {
        double left;
        double right;
        double bottom;
        double top;
        double near;
        double far;
    };

    ViewCamera(CameraMode mode, glm::vec3 camera_position, glm::vec3 target_position, glm::vec3 up_direction);
    CameraMode mode(){return mode_;};
    void applyMatrix();

    virtual void rotate(float delta_x=0, float delta_z=0){};
    virtual void move(float delta_x=0, float delta_y=0){};
    void resetCamera();

    void switchView()
    {
        if (mode_ == kDome){
            view_ =
                    (view_ == View::kOrthogonal) ? View::kPerspective : View::kOrthogonal;
        }
    }
    void setView(float dim_ration);
    void zoom(float zooming_factor);
    void resetView();

    void orthogonalView(){view_ = kOrthogonal;}
    virtual void viewOrtho(DomeCameraRotate direction){};
    std::string getCameraMode();
    std::string getCameraView();
    ViewParams& getCameraViewParams(){return view_params_;}

protected:

    CameraMode mode_;
    View view_{kPerspective};
    ViewParams view_params_ = ViewParams{-1, 1, -1, 1, 1, 50};

    glm::vec3 camera_position_;
    glm::vec3 target_position_;
    glm::vec3 up_direction_;
    std::vector<glm::vec3> initial_coordinates_;

    float yaw_   = glm::radians(90.0f);
    float pitch_ = glm::radians(0.0f);
};

class FirstPersonCamera : public ViewCamera {
public:
    FirstPersonCamera(glm::vec3 camera_position, glm::vec3 target_position, glm::vec3 up_direction);
    void move(float delta_x=0, float delta_z=0) override;
    void rotate(float delta_x=0, float delta_y=0) override;
};

class DomeCamera : public ViewCamera {
public:
    DomeCamera(glm::vec3 camera_position, glm::vec3 target_position, glm::vec3 up_direction, double radius);
    void move(float delta_x=0, float delta_z=0) override;
    void rotate(float delta_x=0, float delta_y=0) override;

    void changeElevation();
    void changeAzimuth();
    void viewOrtho(DomeCameraRotate direction) override;



private:
    double radius_;

};


#endif //PROJECT_2_CAMERA_H
