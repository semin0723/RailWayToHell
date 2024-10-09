#pragma once

#include "yaml-cpp/node/node.h"
#include "yaml-cpp/yaml.h"



using Archive = YAML::Node;


Archive LoadArchive(const char* input);
Archive LoadArchive(std::istream& input);
Archive LoadArchiveFromFile(const std::string& filename);



class ReflectObject;



namespace YAML
{
    template <typename T>
    struct convert;

    template<>
    struct convert<ReflectObject>
    {
        static Node encode(const ReflectObject& rhs);
        static bool decode(const Node& node, ReflectObject& rhs);
    };

}