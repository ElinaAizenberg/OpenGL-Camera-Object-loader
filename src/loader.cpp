
#include <iostream>
#include "tiny_obj_loader.h"
#include "../include/loader.h"

void ObjectLoader::loadObFileData(const std::string &filepath,
                                  std::vector<float> &object_vertices,
                                  std::vector<std::vector<unsigned int>> &object_shapes)
/** Loads vertices and vector of shapes where each shape contains indices using open-source library tiny-obj-loader.*/
{
    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filepath, reader_config))
    {
        if (!reader.Error().empty())
        {
            std::cerr << "TinyObjReader: " << reader.Error();
            throw reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty() && reader.Warning().find("Material") == std::string::npos)
    {
        std::cout << "TinyObjReader: " << reader.Warning();
        throw reader.Warning();
    }

    tinyobj::attrib_t attrib    = reader.GetAttrib();
    std::vector<tinyobj::shape_t> shapes    = reader.GetShapes();

    for (auto const& vertex : attrib.vertices)
    {
        object_vertices.push_back(vertex);
    }

    for (auto const& shape : shapes)
    {
        std::vector<unsigned int> shape_indices;

        for (auto const& index : shape.mesh.indices)
        {
            shape_indices.push_back(index.vertex_index);
        }

        object_shapes.push_back(shape_indices);
    }
}
