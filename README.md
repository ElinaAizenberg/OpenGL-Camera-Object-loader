# OpenGL: object loader, camera system
## Overview
This mini-application demonstrates the implementation of a versatile camera system and a simple .obj file loader (with [TinyObjLoader](https://github.com/tinyobjloader/tinyobjloader)) using OpenGL. Designed as part of a set of mini-applications, its main goal is to illustrate various methods of rendering, object loading, and user interaction within an OpenGL context. The application employs the fixed-function pipeline of OpenGL 3.0 and integrates the Dear ImGui library to provide an intuitive graphical user interface.
It serves as a practical example for beginners to learn and understand the intricacies of camera systems and object handling in 3D graphics. The code is heavily commented, with detailed function descriptions to facilitate learning and ease of understanding.

## Features
- **Load and Scale .obj Files:** easily load any .obj file and automatically scale it to fit the screen for optimal viewing.
- **Save Window Image:** capture and save the current window image to a selected folder.
- **Camera System:**
  - *First Person Camera:* navigate the scene from a first-person perspective.
  - *Dome Camera:* provides an alternative camera mode for different viewing angles.
- **Interactive Controls:**
  - *Mouse:* rotate the camera to explore the 3D scene.
  - *Keyboard:* move the object within the scene.
- **Camera and Projection Switching:** switch between camera modes and orthogonal/perspective projections using keyboard shortcuts.
- **ImGui Integration:**
  - *Camera Settings:* access and modify camera settings through an ImGui window.
  - *Key Reassignment:* customize keyboard controls for various commands within the ImGui interface.
- **Multiple Views:**
  - *Regular View:* a single camera view for standard operations.
  - *Engineering View:* quad-view setup (top, front, side, and regular view) for detailed analysis and manipulation.
- **Ruler Tool:** in Engineering View, apply a ruler tool with a right-click for precise measurements and alignments.

## Screenshots
<img src="https://github.com/user-attachments/assets/889a11ad-2051-49b2-9572-e9881aad1657" width="700">

<img src="https://github.com/user-attachments/assets/89436452-ce39-4269-811c-a98ccaf209fd" width="700">


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
