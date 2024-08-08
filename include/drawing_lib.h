#ifndef PROJECT_2_DRAWING_LIB_H
#define PROJECT_2_DRAWING_LIB_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <tuple>
#include "../include/object.h"
#include "../include/camera.h"

class DrawingLib{
public:
    DrawingLib()= default;

    GLFWwindow* createWindow() const;
    void getWindowSize(GLFWwindow* window);
    std::tuple<int, int> windowSize(){return std::make_tuple(window_width_, window_height_);}

    void drawScene(GLFWwindow* window, Object& object, bool imGuiCaptureMouse);


    void defineCallbackFunction(GLFWwindow* window);
    std::string getCameraMetadata(){return "Camera mode: " + current_camera_->getCameraMode() + "\nCamera view: " + current_camera_->getCameraView();}
    void switchPerspectiveCameraMode()
    {
        current_camera_ = (current_camera_->mode() == kFirstPerson) ? static_cast<ViewCamera*>(&dome_) : static_cast<ViewCamera*>(&fps_);
    }
    void turnOnDomeCamera(){current_camera_ = static_cast<ViewCamera*>(&dome_);}
    void switchCameraView(){
        current_camera_-> switchView();
    }

    void drawRuler();
    void reset();

private:
    int window_width_{1920};
    int window_height_{1080};

    float dim_ratio_ = static_cast<float>(window_height_) / static_cast<float>(window_width_);
    float reference_size_{10.0};

    bool left_button_down_{false};
    bool right_button_down_{false};
    bool imgui_capture_mouse_{false};

    bool ruler_{false};

    double cursor_pos_x_{0}, cursor_pos_y_{0};
    double current_pos_x_{0}, current_pos_y_{0}, prev_pos_x_{0}, prev_pos_y_{0};
    double start_pos_x_{0}, start_pos_y_{0};


    FirstPersonCamera fps_ = FirstPersonCamera(glm::vec3(0.0f, 3.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    DomeCamera dome_       = DomeCamera(glm::vec3(0.0f, 1.0f, 20.0), glm::vec3(0.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 20);
    DomeCamera engineering_camera_       = DomeCamera(glm::vec3(0.0f, 1.0f, 20.0), glm::vec3(0.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 20);

    ViewCamera* current_camera_ = &fps_;


    void drawRegularScene(GLFWwindow* window, Object &object);
    void drawEngineeringScene(GLFWwindow* window, Object &object);

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void scrollCallback(GLFWwindow* window, double yoffset);

    void drawGrid();
    static void drawAxisArrow(float x, float y, float z, const std::string& axis_name);

    std::tuple<double, double> calculateCoordinatesOnMouseMove(int correction_factor) const;
    std::tuple<int, int>  getCurrentViewport(double x_screen, double y_screen) const;

    std::tuple<int, int> current_viewport_;
    std::tuple<double, double> convertCoordinates(double x, double y);
    void printOrthoViewType(int i, int j, DomeCameraRotate ortho_view);
    static std::string OrthViewToString(DomeCameraRotate view) ;
};

#endif //PROJECT_2_DRAWING_LIB_H
