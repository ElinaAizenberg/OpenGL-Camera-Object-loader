
#ifndef PROJECT_2_GUI_H
#define PROJECT_2_GUI_H
#include <functional>

#include "../include/object.h"
#include "../include/drawing_lib.h"


class GuiWindow{
public:
    explicit GuiWindow(Object& object): object_(object){readme_txt_ = readTextFile("../docs/ReadMe.txt");};
    void drawMenu(std::tuple<int, int> window_parameters);
    void drawMainPanel(DrawingLib &drawing_lib);
    void handleShortcuts(std::tuple<int, int> window_parameters);

private:
    Object& object_;
    float window_width_{260};
    float window_height_{500};

    bool animate_{false};
    bool save_image_{false};
    bool help_window_{false};
    bool dummy_bool_{false};

    ImVec2 button_size_ = ImVec2(120,0);

    float window_pos_x_{0};
    float set_cursor_pos_x_{170.0};
    float animation_step_{-3.0};
    int axis_{0};

    std::string readme_txt_;
    std::string rendered_image_path_;

    static void addMenuItem(const std::string& item, const std::string& shortcut, bool &bool_to_update,  const std::function<void()>& func = nullptr);
    static std::string readTextFile(const std::string& filePath);
    void drawHelpWindow();
    void openFile();
    static void exitConfirmMessage();
    void shortcutInput(const std::string& text, const std::string& shortcut_key) const;
    static int inputTextToUpperCaseCallback(ImGuiInputTextCallbackData* data);
    void makePrtSc(int width, int height);
    static void saveRenderedImage(const char* filename, int width, int height);

};

#endif //PROJECT_2_GUI_H
