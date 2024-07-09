#include <iostream>
#include <ctime>
#include <fstream>

#include "imgui.h"
#include "../include/gui.h"
#include "../include/config.h"

#include "portable-file-dialogs.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


void GuiWindow::drawMenu(std::tuple<int, int> window_parameters)
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            std::string open_file_shortcut = "Ctrl+";
            open_file_shortcut += Config::getShortcut("OpenFile");
            if (ImGui::MenuItem("Open", open_file_shortcut.c_str())) {
                openFile();
            }

            std::string save_shortcut = "Ctrl+";
            save_shortcut += Config::getShortcut("SaveImage");
            if (ImGui::MenuItem("Save image", save_shortcut.c_str())) {
                save_image_ = true;
            }

            std::string animate_shortcut = "Ctrl+";
            animate_shortcut += Config::getShortcut("Animate");
            if (ImGui::MenuItem("Hide / Show panel", animate_shortcut.c_str()))
            {
                animate_ = true;
            }

            std::string help_shortcut = "Ctrl+";
            help_shortcut += Config::getShortcut("Help");
            if (ImGui::MenuItem("Help", help_shortcut.c_str()))
            {
                help_window_ = true;
            }

            std::string exit_shortcut = "Ctrl+";
            exit_shortcut += Config::getShortcut("Exit");
            if (ImGui::MenuItem("Exit", exit_shortcut.c_str()))
            {
                exitConfirmMessage();
            }
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

void GuiWindow::drawMainPanel(DrawingLib &drawing_lib)
{
    auto& gui_params = Config::getParameters();

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    float menu_bar_height = ImGui::GetFrameHeight();

    if (animate_)
    {
        if ((animation_step_ < 0 && window_pos_x_ > -window_width_) || (animation_step_ > 0 && window_pos_x_ < 0))
        {
            window_pos_x_ += animation_step_;
        } else {
            animate_ = false;
            animation_step_ *= -1;
        }
    }

    float window_pos_y = viewport->Pos.y + (viewport->Size.y - window_height_) / 2.0f;

    if (window_pos_y < (viewport->Pos.y + menu_bar_height)){
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
    if (ImGui::Button(camera_button.c_str(), button_size_)){
        drawing_lib.switchPerspectiveCameraMode();
    }

    if (camera_button == "FP camera"){
        ImGui::SameLine();
        std::string view_button = (drawing_lib.getCameraMetadata().find("Orthogonal")!= std::string::npos) ? "Perspective" : "Orthogonal";
        if (ImGui::Button(view_button.c_str(), button_size_)){
            drawing_lib.switchCameraView();
        }
    }

    ImGui::Spacing();
    std::string viewport_button = (Config::getParameters().engineering_view_) ? "Regular view" : "Engineering view";
    if (ImGui::Button(viewport_button.c_str(), button_size_)){
        Config::switchEngineeringView();
    }

    ImGui::Spacing();
    if (ImGui::Button("Reset camera", button_size_)){
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
    if (ImGui::TreeNode("General")){
        ShortcutInput("Open file: CTRL + ", "OpenFile");
        ShortcutInput("Save image: CTRL + ", "SaveImage");
        ShortcutInput("Hide/Show panel: CTRL + ", "Animate");
        ShortcutInput("Help: CTRL + ", "Help");
        ShortcutInput("Exit: CTRL + ", "Exit");
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Camera")){
        ShortcutInput("Switch camera mode: ", "SwitchCameraMode");
        ShortcutInput("Switch camera view: ", "SwitchCameraView");
        ShortcutInput("Grid: ", "EnableGrid");
        ShortcutInput("Engineering view: ", "SwitchEngineeringView");

        ImGui::TreePop();
    }

    ImGui::End();

}

int GuiWindow::InputTextToUpperCaseCallback(ImGuiInputTextCallbackData* data) {
    if (data->EventChar >= 'a' && data->EventChar <= 'z') {
        data->EventChar = std::toupper(data->EventChar);
    }
    return 0;
}

void GuiWindow::ShortcutInput(const std::string& text, const std::string& shortcut_key) const{
    ImVec2 startPos = ImGui::GetCursorPos();
    ImGui::Text("%s", text.c_str());

    ImGui::SetCursorPos(ImVec2(startPos.x + set_cursor_pos_x_, startPos.y));

    ImGui::PushItemWidth(25);
    char Shortcut[2] = { Config::getShortcut(shortcut_key), '\0' };

    std::string label = "##"+ shortcut_key;
    if (ImGui::InputText(label.c_str(), Shortcut, IM_ARRAYSIZE(Shortcut), ImGuiInputTextFlags_CallbackCharFilter, InputTextToUpperCaseCallback))
    {
        Config::getShortcut(shortcut_key) = Shortcut[0];
    }
    ImGui::PopItemWidth();
}



void GuiWindow::handleShortcuts(std::tuple<int, int> window_parameters) {
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

void GuiWindow::exitConfirmMessage() {
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


void GuiWindow::openFile() {
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

void GuiWindow::saveRenderedImage(const char* filename, int width, int height) {
    std::vector<unsigned char> pixels(width * height * 3);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Flip the image vertically
    for (int y = 0; y < height / 2; ++y) {
        for (int x = 0; x < width * 3; ++x) {
            std::swap(pixels[y * width * 3 + x], pixels[(height - 1 - y) * width * 3 + x]);
        }
    }

    if (!stbi_write_png(filename, width, height, 3, pixels.data(), width * 3)) {
        std::cerr << "Failed to save image to " << filename << std::endl;
    } else {
        std::cout << "Image saved to " << filename << std::endl;
    }
}

void GuiWindow::makePrtSc(int width, int height) {
    auto folder = pfd::select_folder("Select a folder").result();
    if (!folder.empty()){
        time_t curr_time;
        tm * curr_tm;
        char date_string[100];
        char time_string[100];

        time(&curr_time);
        curr_tm = localtime(&curr_time);

        strftime(date_string, 50, "%Y_%m_%d", curr_tm);
        strftime(time_string, 50, "%H_%M", curr_tm);

        rendered_image_path_ = folder + "/image_" +date_string+"_" + time_string+ ".png";

        saveRenderedImage(rendered_image_path_.c_str(),width, height);

        rendered_image_path_.clear();
        save_image_ = false;
    }
    save_image_ = false;
}

void GuiWindow::drawHelpWindow() {
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
