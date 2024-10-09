#include "ResourceLoader.h"

void ResourceLoader::LoadResource(std::string path)
{
    std::ifstream file(path.c_str());

    if (!file.is_open())
    {
        Assert(false, "failed to open file");
    }

    std::string line;
    std::getline(file, line);
    std::getline(file, line);
    while (std::getline(file, line))
    {
        std::stringstream lineStream(line);
        ResourceInfo info;

        std::getline(lineStream, info.type, ',');
        std::getline(lineStream, info.key, ',');
        std::getline(lineStream, info.path, ',');
        std::getline(lineStream, info.param, ',');

        Load(info);
    }

    file.close();
}

void ResourceLoader::Load(ResourceInfo& info)
{
    if (info.type == "GIF")
    {
        std::wstring path;
        path.assign(info.path.begin(), info.path.end());
        ResourceManager::AddGIFImageByPath(info.key, path);
    }
    else if (info.type == "Image")
    {
        std::wstring path;
        path.assign(info.path.begin(), info.path.end());
        ResourceManager::AddImageByPath(info.key, path);
    }
    else if (info.type == "Font")
    {
        wchar_t tempstr[256];

        MultiByteToWideChar(CP_OEMCP, 0, info.path.c_str(), -1, tempstr, 256);
        ResourceManager::AddFontByPath(info.key, tempstr);
    }
    else if (info.type == "Animation")
    {
        std::shared_ptr<Animation> anim = ResourceManager::AddAnimation(info.key);

        if (info.param == "GIF")
        {
            anim.get()->AddGIF(info.path);
        }
        else if (info.param == "Image")
        {
            anim.get()->AddFrame(info.path);
        }
    }
}
