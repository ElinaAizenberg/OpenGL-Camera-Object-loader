# OpenGL: object loader, camera system
## Overview
A brief description of the project, its purpose, and what it does.

## Features
- 

## Screenshots
<img src="https://github.com/user-attachments/assets/889a11ad-2051-49b2-9572-e9881aad1657" width="500">

<img src="https://github.com/user-attachments/assets/89436452-ce39-4269-811c-a98ccaf209fd" width="500">


## Getting Started
### Prerequisites
- Compiler that supports C++
- CMake (>= 3.10)
- OpenGL (version 3.0)
- GLFW
- GLEW

**External libraries:**
- [Dear ImGui](https://github.com/ocornut/imgui)
- [ImGui Portable File Dialogs](https://github.com/samhocevar/portable-file-dialogs)
- [Image writer stb](https://github.com/nothings/stb/blob/master/stb_image_write.h)
- [Tinyobjloader](https://github.com/tinyobjloader/tinyobjloader)

### Installation
1. Clone the repo
```
git clone https://github.com/ElinaAizenberg/OpenGL-Camera-Object-loader
cd OpenGL-Camera-Object-loader
```

2. In CMakeLists.txt update the path to the folder with external libraries
```
#TODO Specify the directory where imgui and other libraries are located
set(EXTERNAL_LIB_DIR /path_to_directory/libs)
```

3. Build project
```
mkdir build
cd build
cmake ..
make
```

4. Run project
```
./project_2
```



[image_2024_07_09_15_57](https://github.com/ElinaAizenberg/opengl_project_2/assets/77394738/0dbfd802-b2e0-4525-93e3-15e20852816c)
