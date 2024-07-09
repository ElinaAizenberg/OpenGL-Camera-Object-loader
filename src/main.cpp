#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "../include/object.h"
#include "../include/drawing_lib.h"
#include "../include/gui.h"
#include "../include/config.h"

Parameters Config::parameters_;
std::map<std::string, char> Config::shortcuts_ = {
        {"Animate", 'A'},
        {"Exit", 'Q'},
        {"OpenFile", 'O'},
        {"SaveImage", 'S'},
        {"Help", 'H'},
        {"SwitchCameraMode", '0'},
        {"SwitchCameraView", '1'},
        {"EnableGrid", '3'},
        {"SwitchEngineeringView", '4'}
};


int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Session
    Object object = Object();
    DrawingLib drawing_lib    = DrawingLib();
    GuiWindow gui_window = GuiWindow(object);

    GLFWwindow* window = drawing_lib.createWindow();
    glfwMakeContextCurrent(window);
    drawing_lib.defineCallbackFunction(window);

    GLenum res = glewInit();
    if (res)
    {
        std::cout << glewGetErrorString(res) << std::endl;
    }

    if (res != GLEW_OK)
    {
        std::terminate();
    }
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    ImGui::StyleColorsDark();

    ImGui_ImplOpenGL3_CreateFontsTexture();

    object.loadObjectFile("../objects/bunny.obj");

    while (glfwWindowShouldClose(window) == 0)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        drawing_lib.getWindowSize(window);

        auto window_parameters = drawing_lib.windowSize();

        gui_window.drawMenu(window_parameters);
        gui_window.handleShortcuts(window_parameters);
        gui_window.drawMainPanel(drawing_lib);

        // Check if ImGui wants to capture the mouse
        bool ioWantCaptureMouse = ImGui::GetIO().WantCaptureMouse;

        drawing_lib.drawScene(window, object, ioWantCaptureMouse);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        glfwPollEvents();
        GLuint res = glGetError();
        if (res)
        {
            std::cout << glewGetErrorString(res) << std::endl;
        }

    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
