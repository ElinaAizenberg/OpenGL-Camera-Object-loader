
#ifndef PROJECT_2_GUI_H
#define PROJECT_2_GUI_H

#include "../include/object.h"
#include "../include/drawing_lib.h"



class GuiWindow{
public:
    explicit GuiWindow(Object& object): object_(object){readme_txt_ = readTextFile("../ReadMe.txt");};
    void drawMenu(std::tuple<int, int> window_parameters);
    void drawHelpWindow();
    void drawMainPanel(DrawingLib &drawing_lib);
    void saveRenderedImage(const char* filename, int width, int height);
    void handleShortcuts(std::tuple<int, int> window_parameters);
    void makePrtSc(int width, int height);


private:
    Object& object_;
    float window_width_ = 260;
    float window_height_ = 500;
    ImVec2 button_size_ = ImVec2(120,0);

    float window_pos_x_{0};
    bool animate_ = false;
    float animation_step_{-3.0};
    int axis_ = 0;
    float set_cursor_pos_x_{170.0};

    std::string rendered_image_path_;
    bool save_image_{false};
    bool help_window_{false};
    std::string readme_txt_;


    static void exitConfirmMessage();
    void openFile();
    void ShortcutInput(const std::string& text, const std::string& shortcut_key) const;
    static int InputTextToUpperCaseCallback(ImGuiInputTextCallbackData* data);
    static std::string readTextFile(const std::string& filePath);

};

#endif //PROJECT_2_GUI_H
