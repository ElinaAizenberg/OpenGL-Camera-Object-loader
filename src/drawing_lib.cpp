
#include <algorithm>
#include <tuple>
#include <glm/glm.hpp>
#include "stb_image_write.h"
#include "../include/drawing_lib.h"
#include "../include/font.h"
#include "../include/config.h"


GLFWwindow* DrawingLib::createWindow() const
/** Creates and returns a new GLFW window with the specified width, height, and title. */
{
    return glfwCreateWindow(window_width_, window_height_, "OpenGL Project 1", nullptr, nullptr);
}

void DrawingLib::getWindowSize(GLFWwindow* window)
/** Retrieves the size of the specified GLFW window and updates the clas variables for width, height, and dimension ratio. */
{
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    window_width_  = w;
    window_height_ = h;
    dim_ratio_ = static_cast<float>(window_height_) / static_cast<float>(window_width_);
}

void DrawingLib::drawScene(GLFWwindow* window, Object &object, bool imGuiCaptureMouse)
/** Renders the scene based on the current configuration, either in engineering or regular view. */
{
    imgui_capture_mouse_ = imGuiCaptureMouse;

    if (Config::getParameters().engineering_view_)
    {
        drawEngineeringScene(window, object);
    }
    else
    {
        drawRegularScene(window, object);
    }
}

void DrawingLib::drawRegularScene(GLFWwindow* window, Object &object)
/** Renders the scene using the regular view configuration. */
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Viewport is the region of the window where the rendered image is displayed.
    //It's specified in screen coordinates, with (0, 0) being the bottom-left corner of the window
    glViewport(0, 0, window_width_, window_height_);

    // Switches the current matrix mode to the projection matrix.
    // It indicates that subsequent matrix operations (like glLoadIdentity(), glOrtho(), glFrustum(), etc.)
    // will affect the projection matrix.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    current_camera_->setView(dim_ratio_);

    // After setting up the projection matrix, switches to GL_MODELVIEW mode to handle model transformations.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    current_camera_->applyMatrix();

    if (Config::getParameters().grid_)
    {
        drawGrid();
    }

    float scalingFactor = object.calculateScalingFactor(reference_size_);
    glScalef(scalingFactor, scalingFactor, scalingFactor);

    object.draw();
}

void DrawingLib::drawEngineeringScene(GLFWwindow* window, Object &object)
/** Renders the scene using the Engineering view configuration. */
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Free view uses Dome camera by default.
    turnOnDomeCamera();
    // Engineering view assumes orthogonal projection.
    engineering_camera_.orthogonalView();

    // each view (Front, Top, Side and Free) is rendered individually.
    for (int i = 0; i< 2; i++)
        {
        for (int j = 0; j< 2; j++)
            {
            DomeCameraRotate ortho_view;
            dim_ratio_ = static_cast<float>(window_height_/2) / static_cast<float>(window_width_/2);
            glViewport(i * (window_width_/2), j * (window_height_/2), window_width_/2, window_height_/2);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();

            if (i == 0 && j == 0)
            {
                ortho_view = kFront;
            }
            else if (i == 1 && j == 0){
                ortho_view = kSide;
            }
            else if (i == 0 && j == 1){
                ortho_view = kTop;
            }
            else{
                ortho_view = kFree;
            }

            if (ortho_view == kFree)
            {
                current_camera_ ->setView(dim_ratio_);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                current_camera_-> applyMatrix();
            }
            else {
                engineering_camera_.setView(dim_ratio_);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                engineering_camera_.viewOrtho(ortho_view);
            }
            // ruler cannot be used in Free view section.
            if ((std::get<0>(current_viewport_) == i && std::get<1>(current_viewport_) == j) || (i==1 && j == 1)){
                if (ruler_){
                    drawRuler();
                }
            }

            if (Config::getParameters().grid_)
            {
                drawGrid();
            }

            float scalingFactor = object.calculateScalingFactor(reference_size_);
            glScalef(scalingFactor, scalingFactor, scalingFactor);

            object.draw();

            printOrthoViewType(i, j, ortho_view);
        }

    }
}


void DrawingLib::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
/** Handles mouse button events in a GLFW window. If the cursor position is not on any of ImGui elements,
it performs actions on left-click, double left-click and right-click. */
{
    // this boolean is initialized in main.py and checks if mouse position is on any of ImGui elements
    if (!imgui_capture_mouse_)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            left_button_down_ = true;
            glfwGetCursorPos(window, &cursor_pos_x_, &cursor_pos_y_);
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {
            left_button_down_   = false;
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            right_button_down_ = true;
            glfwGetCursorPos(window, &cursor_pos_x_, &cursor_pos_y_);
            current_viewport_ = getCurrentViewport(current_pos_x_, current_pos_y_);
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        {
            right_button_down_   = false;
            ruler_ = false;
        }
    }
}


void DrawingLib::printOrthoViewType(int i, int j, DomeCameraRotate ortho_view)
/** Prints the name of the view (Front, Top, Side) when Engineering view is on. */
{
    glClear(GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(-1, 1,
            -1 * dim_ratio_, 1 * dim_ratio_,
            -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glColor3f(1,1,1);  // Sets the colour to white.
    glRasterPos2f(0.8*dim_ratio_ , -0.8*dim_ratio_);
    print_string(OrthViewToString(ortho_view).c_str());
}


void DrawingLib::cursorPositionCallback(GLFWwindow* window,
                                        double input_cursor_pos_x,
                                        double input_cursor_pos_y)
/** Handles cursor movement events in a GLFW window.*/
{
    prev_pos_x_    = current_pos_x_;
    prev_pos_y_    = current_pos_y_;
    current_pos_x_ = input_cursor_pos_x;
    current_pos_y_ = input_cursor_pos_y;

    if (left_button_down_)
    {
        auto delta_coordinates = calculateCoordinatesOnMouseMove(2);
        current_camera_->rotate(std::get<0>(delta_coordinates), std::get<1>(delta_coordinates));
    }
    if (right_button_down_)
    {
        // ruler is turned on only in engineering view and for Front, Top, Side views (not Free view).
        if (!ruler_)
        {
            if (current_camera_->mode() == kDome && !(std::get<0>(current_viewport_) == 1 && std::get<1>(current_viewport_) == 1))
            {
                ruler_ = true;
                start_pos_x_ = current_pos_x_;
                start_pos_y_ = current_pos_y_;
            }
        }
    }
}

void DrawingLib::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
/** Handles keyboard events in a GLFW window.*/
{
    if (action == GLFW_PRESS)
    {
        auto shortcut_keys = Config::getShortcuts();
        char key_char = static_cast<char>(key);
        if (shortcut_keys["SwitchCameraMode"] == key_char)
        {
            switchPerspectiveCameraMode();  // switch between First person camera and Dome camera
        }
        if (shortcut_keys["SwitchCameraView"] == key_char)
        {
            switchCameraView();  // switch between Orthogonal view and Perspective view
        }
        if (shortcut_keys["EnableGrid"] == key_char)
        {
            Config::switchGrid();  // enable/disable grid
        }
        if (shortcut_keys["SwitchEngineeringView"] == key_char)
        {
            Config::switchEngineeringView(); // enable/disable Engineering view
        }
        if (key == GLFW_KEY_LEFT)
        {
            current_camera_->move(1,0);
        }
        if (key == GLFW_KEY_RIGHT)
        {
            current_camera_->move(-1,0);
        }
        if (key == GLFW_KEY_UP)
        {
            current_camera_->move(0,1);
        }
        if (key == GLFW_KEY_DOWN)
        {
            current_camera_->move(0,-1);
        }
        if (key == GLFW_KEY_HOME)
        {
            reset();
        }
    }
}

void DrawingLib::scrollCallback(GLFWwindow* window, double yoffset)
/** Callback function that handles scroll input from the mouse wheel to zoom in or out of the scene.
If ImGui is not capturing the mouse input and the zoom level is adjusted based on the scroll direction. */
{
    if (!imgui_capture_mouse_)
        {
            if (yoffset > 0)
            {
                current_camera_->zoom(0.1);   // zoom-in
                engineering_camera_.zoom(0.1);
            }
            else if (yoffset < 0)
            {
                current_camera_->zoom(-0.1);   // zoom-out
                engineering_camera_.zoom(-0.1);
            }
    }
}

void DrawingLib::defineCallbackFunction(GLFWwindow* window)
/** Sets up callback functions for a GLFW window.*/
{
    // Sets a custom user-defined pointer (current instance of DrawingLib) to be associated with the GLFW window.
    // It allows the instance's methods to be easily accessed within the GLFW callbacks, enabling event handling
    // (mouse clicks, cursor movements, and scrolling) to interact with the DrawingLib instance.
    glfwSetWindowUserPointer(window, this);

    // These callbacks enable interaction with the window using the mouse for actions such as clicking, dragging, and scrolling.
    glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods) {
        auto* drawing_lib = static_cast<DrawingLib*>(glfwGetWindowUserPointer(win));
        drawing_lib->mouseButtonCallback(win, button, action, mods);
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* win, double xpos, double ypos) {
        auto* drawing_lib = static_cast<DrawingLib*>(glfwGetWindowUserPointer(win));
        drawing_lib->cursorPositionCallback(win, xpos, ypos);
    });

    glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
        auto* drawing_lib = static_cast<DrawingLib*>(glfwGetWindowUserPointer(win));
        drawing_lib->keyCallback(win, key, scancode, action, mods);
    });

    glfwSetScrollCallback(window, [](GLFWwindow* win, double xoffset, double yoffset) {
        auto* drawing_lib = static_cast<DrawingLib*>(glfwGetWindowUserPointer(win));
        drawing_lib->scrollCallback(win, yoffset);
    });
}


void DrawingLib::drawGrid()
/** Draws grid in steps: xz-plane with grid frequency defined in Config class, arrow for X-, Y-, Z-axis,
coordinates of the maximum coordinate of xz-plane.*/
{
    auto grid_params = Config::getParameters();
    glBegin(GL_LINES);

    for (int i = 0; i <= static_cast<int>(grid_params.grid_end_ / grid_params.grid_frequency_); ++i)
    {
        float value = i * grid_params.grid_frequency_;

        glColor3f(.25, .25, .25);
        glVertex3f(value, 0, 0);
        glVertex3f(value, 0, -grid_params.grid_end_);

        glVertex3f(0, 0, -value);
        glVertex3f(grid_params.grid_end_, 0, -value);
    }

    glEnd();

    drawAxisArrow(grid_params.grid_end_,0,0, "X");
    drawAxisArrow(0,grid_params.grid_end_,0, "Y");
    drawAxisArrow(0,0,-grid_params.grid_end_, "-Z");

    glColor3f(.25, .25, .25);
    glRasterPos3f(grid_params.grid_end_ , 0, -grid_params.grid_end_);
    int end_int = static_cast<int>(grid_params.grid_end_);
    std::string end_coordinates = std::to_string(end_int) + ", 0, -" + std::to_string(end_int);
    print_string(end_coordinates.c_str());
}


void DrawingLib::drawAxisArrow(float x, float y, float z, const std::string& axis_name)
/** Draws axis arrow in steps: axis line, arrow and prints axis name.*/
{
    float arrowSize = 0.05f;
    float rgb[] = {0,0,0};

    if (x > 0)
    {
        rgb[0] = 255; // color for X-axis
    }
    else if (y > 0)
    {
        rgb[1] = 255; // color for Y-axis
    }
    else{
        rgb[2] = 255; // color for Z-axis
    }
    // size of the arrow is proportional to the length of the axis
    float height = std::sqrt(std::pow((x*arrowSize),2) + std::pow((y*arrowSize),2) + std::pow((z*arrowSize),2));

    glColor3f(rgb[0], rgb[1], rgb[2]);

    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(x, y, z);

    glVertex3f(0, 0, 0);
    glVertex3f(-x, -y, -z);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLES);

    glVertex3f(x+x*arrowSize, y+y*arrowSize, z + z*arrowSize);
    if (x>0)
    {
        glVertex3f(x, y, z+(height/2));
        glVertex3f(x, y, z-(height/2));
    }
    else{
        glVertex3f(x+(height/2), y, z);
        glVertex3f(x-(height/2), y, z);
    }

    glEnd();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glColor3f(1,1,1);
    glRasterPos3f(x+x*arrowSize , y+y*arrowSize, z + z*arrowSize);
    print_string(axis_name.c_str());
}

std::tuple<double, double> DrawingLib::calculateCoordinatesOnMouseMove(int correction_factor) const
/** Calculates the change in object coordinates of mouse cursor, converting screen space coordinates to
 normalized device coordinates (NDC) and then to frustum coordinates, with depth correction applied. */
{
    // Convert the mouse movement (delta_x and delta_y) from screen space to normalized device coordinates (NDC)
    // where the range is [-1, 1].
    double ndc_delta_x = (current_pos_x_ - prev_pos_x_) / (window_width_ / 2.0f);
    double ndc_delta_y = (current_pos_y_ - prev_pos_y_) / (window_height_ / 2.0f);

    // Compute the width and height of the frustum at the near plane
    double frustum_width = 1 - (-1);
    double frustum_height = (1 * dim_ratio_) - (-1 * dim_ratio_);

    // Scale normalized deltas to match the frustum coordinates
    double adjusted_delta_x = ndc_delta_x * frustum_width / 2.0f;
    double adjusted_delta_y = ndc_delta_y * frustum_height / 2.0f;

    // Apply depth correction factor (assuming linear scaling for simplicity)
    adjusted_delta_x *= correction_factor;
    adjusted_delta_y *= correction_factor;

    std::tuple<double, double> delta_coordinates = std::make_tuple(adjusted_delta_x, adjusted_delta_y);

    return delta_coordinates;
}


void DrawingLib::drawRuler()
/** Draws ruler in Engineering view. In any of Front, Top, Side views ruler is turned on with right mouse click.
Start, end coordinates and length of the draw vector are displayed as well.*/
{
    auto start = convertCoordinates(start_pos_x_, start_pos_y_);
    double adjusted_start_x = std::get<0>(start);
    double adjusted_start_y = std::get<1>(start);

    auto end = convertCoordinates(current_pos_x_, current_pos_y_);
    double adjusted_current_x = std::get<0>(end);
    double adjusted_current_y = std::get<1>(end);

    // depends on the view and which plane (xy, xz or yz) is nor displayed, the corresponding coordinate is set to 0.
    double x1,y1,z1, x2, y2, z2;
    if (std::get<0>(current_viewport_) == 0 && std::get<1>(current_viewport_) == 0){
        x1 =adjusted_start_x;
        x2 = adjusted_current_x;
        y1 = adjusted_start_y;
        y2 = adjusted_current_y;
        z1 = 0;
        z2 = 0;
    }
    if (std::get<0>(current_viewport_) == 0 && std::get<1>(current_viewport_) == 1){
        x1 = adjusted_start_x;
        x2 = adjusted_current_x;
        y1 = 0;
        y2 = 0;
        z1 = -adjusted_start_y;
        z2 = -adjusted_current_y;
    }
    if (std::get<0>(current_viewport_) == 1 && std::get<1>(current_viewport_) == 0){
        x1 = 0;
        x2 = 0;
        y1 = adjusted_start_y;
        y2 = adjusted_current_y;
        z1 = -adjusted_start_x;
        z2 = -adjusted_current_x;
    }

    glColor3f(1,1,0);
    glBegin(GL_LINES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glEnd();

    glPointSize(5.0f);

    glBegin(GL_POINTS);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glEnd();

    std::string string_start = "(" + std::to_string(int(x1)) + ", " + std::to_string(int(y1)) + ", " + std::to_string(int(z1)) + ")";

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glRasterPos3f(x1, y1, z1);
    print_string(string_start.c_str());

    std::string string_end = "(" + std::to_string(int(x2)) + ", " + std::to_string(int(y2)) + ", " + std::to_string(int(z2)) + ")";
    glRasterPos3f(x2, y2, z2);
    print_string(string_end.c_str());

    int length = std::sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1));
    std::string string_length = std::to_string(length);
    glRasterPos3f((x2+x1)/2, (y2+y1)/2, (z2+z1)/2);
    print_string(string_length.c_str());
}

std::tuple<int, int>  DrawingLib::getCurrentViewport(double x_screen, double y_screen) const
/** Determines the viewport indices (i, j) by dividing the screen coordinates by half the window width and height,
effectively identifying the viewport quadrant where the given screen coordinates reside. It is applied when
Engineering view is turned on. */
{
    int viewport_width = window_width_ / 2;
    int viewport_height = window_height_ / 2;

    int i = static_cast<int>(x_screen) / viewport_width;
    int j = static_cast<int>(window_height_ - y_screen) / viewport_height;

    return std::make_tuple(i, j);
}

std::tuple<double, double> DrawingLib::convertCoordinates(double x, double y)
/** Converts screen coordinates to world coordinates based on the camera view parameters and orthographic coefficient. */
{
    auto orth_c = Config::getParameters().ortho_coefficient_;
    if (x > window_width_/2)
    {
        x = x- window_width_/2;
    }
    if (y > window_height_/2)
    {
        y = y- window_height_/2;
    }

    double a,b,p,q;
    p = 2*x / window_width_;
    q = 2*y / window_height_;

    a = (1-p)*engineering_camera_.getCameraViewParams().left * orth_c +p*engineering_camera_.getCameraViewParams().right * orth_c;
    b = (1-q)*(engineering_camera_.getCameraViewParams().top* orth_c * dim_ratio_) + q*(engineering_camera_.getCameraViewParams().bottom* orth_c * dim_ratio_);

    return std::make_tuple(a , b);
}

std::string DrawingLib::OrthViewToString(DomeCameraRotate view)
/** Returns a string that describes the orthographic view based on the enum value, such as "Front view" or "Top view". */
{
    switch (view)
    {
        case kFront: return "Front view";
        case kTop: return "Top view";
        case kSide: return "Side view";
        case kFree: return "Free view";
        default: return "Unknown";
    }
}

void DrawingLib::reset()
/** Resets Camera settings to its initial values.*/
{
    current_camera_->resetCamera();
    current_camera_->resetView();

    engineering_camera_.resetCamera();
    engineering_camera_.resetView();
}
