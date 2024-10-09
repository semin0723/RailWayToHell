#include "FileManager.h"
#include "Object.h"


void FileManager::WriteObjectBinaryFile(Object* object, std::string path)
{
	std::ofstream fileWrite(path.c_str(), std::ios::binary);
	if (!fileWrite)
	{
		Assert(false, "failed to open file for writing.");
		return;
	}

	fileWrite.write(reinterpret_cast<char*>(object), sizeof(*object));
}

void FileManager::ReadObjectBinaryFile(std::string path)
{
	std::ifstream fileRead(path.c_str(), std::ios::binary);
	if (!fileRead)
	{
		Assert(false, "failed to open file for reading.");
		return;
	}

	//fileRead.read();
}
