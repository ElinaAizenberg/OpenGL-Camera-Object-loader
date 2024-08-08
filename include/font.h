#ifndef PROJECT_2_FONT_H
#define PROJECT_2_FONT_H
#include <GLFW/glfw3.h>
#include <cstring>


const uint32_t kFontHeight{13};

extern GLubyte space[];
extern GLubyte rasters[][kFontHeight];

const GLubyte* getBitmapForCharacter(char c);
void print_string(const char *s);


#endif //PROJECT_2_FONT_H
