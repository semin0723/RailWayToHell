#include "Archive.h"
#include "yaml-cpp/node/parse.h"
#include "yaml-cpp/node/convert.h"
#include "yaml-cpp/node/emit.h"
#include "ReflectObject.h"
#include <sstream>

Archive LoadArchive(const char* input)
{
	return YAML::Load(input);
}

Archive LoadArchive(std::istream& input)
{
    return YAML::Load(input);
}

Archive LoadArchiveFromFile(const std::string& filename)
{
    return YAML::LoadFile(filename);
}

namespace YAML
{
    Node convert<ReflectObject>::encode(const ReflectObject& rhs)
    {
        Node node;
        rhs.Serialize(node);
        return node;
    }

    bool convert<ReflectObject>::decode(const Node& node, ReflectObject& rhs)
    {
        return false;
    }

}



