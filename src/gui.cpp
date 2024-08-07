#include <iostream>
#include <ctime>
#include <fstream>

#include "imgui.h"
#include "portable-file-dialogs.h"

#include "../include/gui.h"
#include "../include/config.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


void GuiWindow::drawMenu(std::tuple<int, int> window_parameters)
/** Draws the main Menu with several items that can be called with shortcuts as well.
If corresponding booleans are true, saves image of the screen in the selected folder and opens Help window.*/
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            addMenuItem("Open", "OpenFile", dummy_bool_, [this](){openFile();});
            addMenuItem("Save image", "SaveImage", save_image_);
            addMenuItem("Hide / Show panel", "Animate", animate_);
            addMenuItem("Help", "Help", help_window_);
            addMenuItem("Exit", "Exit", dummy_bool_, [this](){exitConfirmMessage();});
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    if (save_image_){
        makePrtSc(std::get<0>(window_parameters), std::get<1>(window_parameters));
    }
    if (help_window_){
        drawHelpWindow();
    }
}

void GuiWindow::addMenuItem(const std::string &item, const std::string &shortcut, bool &bool_to_update,  const std::function<void()>& func)
/** Creates a Main menu item with shortcut. If boolean is provided, switches it to true.
If function object is provided, calls it.*/
{
    std::string open_file_shortcut = "Ctrl+";
    open_file_shortcut += Config::getShortcut(shortcut);
    if (ImGui::MenuItem(item.c_str(), open_file_shortcut.c_str()))
    {
        if (!bool_to_update){
            bool_to_update = true;
        }
        if (func){
            func();
        }
    }
}

void GuiWindow::drawMainPanel(DrawingLib &drawing_lib)
/** Draws Settings window with x_position = 0 (the most left) and y_position calculated at the center of left side.
Whenever animation is called via Main menu or short-cut (Ctrl+A), Settings is moved out/in. */
{
    auto& gui_params = Config::getParameters();

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    float menu_bar_height = ImGui::GetFrameHeight();

    if (animate_)
    {
        // if animation step is negative (moves window out), still seen on the screen OR
        // if animation step is positive (moves window in), x_position of window is still out of screen
        if ((animation_step_ < 0 && window_pos_x_ > -window_width_) || (animation_step_ > 0 && window_pos_x_ < 0))
        {
            // move Settings window position by animation step every frame
            window_pos_x_ += animation_step_;
        }
        else
        {
            // disable window animation and switch animation direction for the next time
            animate_ = false;
            animation_step_ *= -1;
        }
    }

    // calculate y_position to set Settings window in the center of vertical side of the viewport
    float window_pos_y = viewport->Pos.y + (viewport->Size.y - window_height_) / 2.0f;
    if (window_pos_y < (viewport->Pos.y + menu_bar_height))
    {
        window_pos_y = viewport->Pos.y + menu_bar_height;
    }

    ImGui::SetNextWindowPos(ImVec2(window_pos_x_, window_pos_y));
    ImGui::SetNextWindowSize(ImVec2(window_width_, window_height_));

    auto flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

    ImGui::Begin("Settings", nullptr, flags);

    ImGui::Spacing();
    ImGui::SeparatorText("Camera");
    ImGui::Text("Camera metadata:");
    ImGui::Text("%s", drawing_lib.getCameraMetadata().c_str());
    ImGui::Spacing();

    std::string camera_button = (drawing_lib.getCameraMetadata().find("Dome")!= std::string::npos) ? "FP camera" : "Dome camera";
    if (ImGui::Button(camera_button.c_str(), button_size_))
    {
        drawing_lib.switchPerspectiveCameraMode();
    }

    if (camera_button == "FP camera")
    {
        ImGui::SameLine();
        std::string view_button = (drawing_lib.getCameraMetadata().find("Orthogonal")!= std::string::npos) ? "Perspective" : "Orthogonal";
        if (ImGui::Button(view_button.c_str(), button_size_))
        {
            drawing_lib.switchCameraView();
        }
    }

    ImGui::Spacing();
    std::string viewport_button = (Config::getParameters().engineering_view_) ? "Regular view" : "Engineering view";
    if (ImGui::Button(viewport_button.c_str(), button_size_))
    {
        Config::switchEngineeringView();
    }

    ImGui::Spacing();
    if (ImGui::Button("Reset camera", button_size_))
    {
        drawing_lib.reset();
    }

    ImGui::Spacing();
    ImGui::SeparatorText("Grid");

    ImGui::Checkbox(" show grid", &gui_params.grid_);
    ImGui::Spacing();
    ImGui::SliderFloat("##grid frequency", &gui_params.grid_frequency_, 0.1f, 2.5f, "frequency = %.1f");
    ImGui::SliderFloat("##grid end", &gui_params.grid_end_, 1.0f, 15.0f, "maximum = %1.0f");


    ImGui::Spacing();
    ImGui::SeparatorText("Object");
    ImGui::Text("Rotate object (90 degrees): ");

    ImGui::RadioButton("X-axis", &axis_, 0); ImGui::SameLine();
    ImGui::RadioButton("Y-axis", &axis_, 1); ImGui::SameLine();
    ImGui::RadioButton("Z-axis", &axis_, 2);

    if (ImGui::ArrowButton("##left", ImGuiDir_Left))
    { object_.rotateObjects(axis_, 1);}
    ImGui::SameLine();
    if (ImGui::ArrowButton("##right", ImGuiDir_Right))
    { object_.rotateObjects(axis_, -1);}

    ImGui::Spacing();
    ImGui::SeparatorText("Shortcuts");
    if (ImGui::TreeNode("General"))
    {
        shortcutInput("Open file: CTRL + ", "OpenFile");
        shortcutInput("Save image: CTRL + ", "SaveImage");
        shortcutInput("Hide/Show panel: CTRL + ", "Animate");
        shortcutInput("Help: CTRL + ", "Help");
        shortcutInput("Exit: CTRL + ", "Exit");
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Camera"))
    {
        shortcutInput("Switch camera mode: ", "SwitchCameraMode");
        shortcutInput("Switch camera view: ", "SwitchCameraView");
        shortcutInput("Grid: ", "EnableGrid");
        shortcutInput("Engineering view: ", "SwitchEngineeringView");

        ImGui::TreePop();
    }
    ImGui::End();
}

int GuiWindow::inputTextToUpperCaseCallback(ImGuiInputTextCallbackData* data)
/** Callback function that applies upper case to lower case chars.*/
{
    if (data->EventChar >= 'a' && data->EventChar <= 'z')
    {
        data->EventChar = std::toupper(data->EventChar);
    }
    return 0;
}

void GuiWindow::shortcutInput(const std::string& text, const std::string& shortcut_key) const
/** Creates a set of a string with short-cut name and input field to assign a char to a short-cut items.*/
{
    ImVec2 startPos = ImGui::GetCursorPos();
    ImGui::Text("%s", text.c_str());

    ImGui::SetCursorPos(ImVec2(startPos.x + set_cursor_pos_x_, startPos.y));

    ImGui::PushItemWidth(25);
    char Shortcut[2] = { Config::getShortcut(shortcut_key), '\0' };

    std::string label = "##"+ shortcut_key;
    if (ImGui::InputText(label.c_str(), Shortcut, IM_ARRAYSIZE(Shortcut), ImGuiInputTextFlags_CallbackCharFilter, inputTextToUpperCaseCallback))
    {
        Config::getShortcut(shortcut_key) = Shortcut[0];
    }
    ImGui::PopItemWidth();
}

void GuiWindow::handleShortcuts(std::tuple<int, int> window_parameters)
/** Sets up callback functions for ImGui short-cuts..*/
{
    ImGuiIO& io = ImGui::GetIO();

    if (io.KeyCtrl && ImGui::IsKeyPressed(static_cast<ImGuiKey>(Config::getShortcut("Animate"))))
    {
        animate_ = true;
    }
    if (io.KeyCtrl && ImGui::IsKeyPressed(static_cast<ImGuiKey>(Config::getShortcut("OpenFile"))))
    {
        openFile();
    }
    if (io.KeyCtrl && ImGui::IsKeyPressed(static_cast<ImGuiKey>(Config::getShortcut("Help"))))
    {
        help_window_ = true;
    }
    if (io.KeyCtrl && ImGui::IsKeyPressed(static_cast<ImGuiKey>(Config::getShortcut("SaveImage"))))
    {
        makePrtSc(std::get<0>(window_parameters), std::get<1>(window_parameters));
    }
    if (io.KeyCtrl && ImGui::IsKeyPressed(static_cast<ImGuiKey>(Config::getShortcut("Exit"))))
    {
        exitConfirmMessage();
    }
}

void GuiWindow::exitConfirmMessage()
/** Displays a confirmation dialog asking if the user wants to exit, exits the program if 'Yes' is selected. */
{
    auto button = pfd::message("Action requested", "Are you sure you want to exit?",
                               pfd::choice::yes_no, pfd::icon::question).result();

    switch (button)
    {
        case pfd::button::yes:
            exit(0);
        case pfd::button::no:
            break;
    }
}

void GuiWindow::openFile()
/** Opens a file dialog to select an .obj file and loads it, notifies the user if no file is selected.*/
{
    auto selection = pfd::open_file("Select a file", ".",
                                    { "Object Files", "*.obj"}).result();
    if (!selection.empty())
    {
        object_.loadObjectFile(selection[0]);
    }
    else
    {
        pfd::notify("System event", "An .obj file was not selected.",
                    pfd::icon::info);
    }
}

void GuiWindow::saveRenderedImage(const char* filename, int width, int height)
/** Creates a .png file with the screen image.*/
{
    std::vector<unsigned char> pixels(width * height * 3);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Flip the image vertically
    for (int y = 0; y < height / 2; ++y) {
        for (int x = 0; x < width * 3; ++x) {
            std::swap(pixels[y * width * 3 + x], pixels[(height - 1 - y) * width * 3 + x]);
        }
    }

    if (!stbi_write_png(filename, width, height, 3, pixels.data(), width * 3))
    {
        std::cerr << "Failed to save image to " << filename << std::endl;
    } else
    {
        std::cout << "Image saved to " << filename << std::endl;
    }
}

void GuiWindow::makePrtSc(int width, int height)
/** Makes print-screen and saves .png file into the selected folder with saveRenderedImage function.*/
{
    auto folder = pfd::select_folder("Select a folder").result();

    if (!folder.empty())
    {
        time_t curr_time;
        tm * curr_tm;
        char date_string[50];

        time(&curr_time);
        curr_tm = localtime(&curr_time);

        strftime(date_string, sizeof(date_string), "%Y_%m_%d_%H_%M", curr_tm);
        rendered_image_path_ = folder + "/image_" + date_string + ".png";
        saveRenderedImage(rendered_image_path_.c_str(),width, height);
        rendered_image_path_.clear();
    }
    save_image_ = false;
}

void GuiWindow::drawHelpWindow()
/** Prints README.txt file content with information related to all functionality in this project in n individual window. */
{
    ImGui::Begin("Help", &help_window_);
    ImGui::TextWrapped("%s", readme_txt_.c_str());
    ImGui::End();
}

std::string GuiWindow::readTextFile(const std::string &filePath)
/** Reads the contents of a text file into a string. */
{
    std::ifstream file(filePath);
    std::stringstream buffer;

    if (file.is_open())
    {
        buffer << file.rdbuf();
        file.close();
    } else {
        return "Failed to open the file.";
    }

    return buffer.str();
}

