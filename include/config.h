#ifndef PROJECT_2_CONFIG_H
#define PROJECT_2_CONFIG_H


#include <map>

struct Parameters
{
    bool engineering_view_{false};
    bool grid_{false};
    float grid_frequency_{1.0};
    float grid_end_{10};
    double ortho_coefficient_{15};

};

class Config
/** Config class is used across the whole application to get access to Parameters. */
{
public:
    static Parameters& getParameters(){return parameters_;}
    static std::map<std::string, char>& getShortcuts(){return shortcuts_;}
    static char& getShortcut(const std::string& key){return shortcuts_[key];}

    static void switchGrid(){parameters_.grid_ = !parameters_.grid_;}
    static void switchEngineeringView()
    {
        if (!parameters_.engineering_view_){
            parameters_.grid_ = true;
            parameters_.ortho_coefficient_ = 20;
        }
        else {
            parameters_.ortho_coefficient_ = 15;

        }
        parameters_.engineering_view_ = !parameters_.engineering_view_;
    }


private:
    static Parameters parameters_;
    static std::map<std::string, char> shortcuts_;
};

#endif //PROJECT_2_CONFIG_H
