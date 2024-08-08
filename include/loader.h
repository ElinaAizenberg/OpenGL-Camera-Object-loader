#ifndef PROJECT_2_LOADER_H
#define PROJECT_2_LOADER_H

#include <string>
#include <vector>


class ObjectLoader
{
public:
    static void loadObFileData(const std::string &filepath,
                               std::vector<float> &object_vertices,
                               std::vector<std::vector<unsigned int>> &object_shapes);

};

#endif //PROJECT_2_LOADER_H
