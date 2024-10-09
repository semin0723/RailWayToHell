#pragma once
#include <fstream>
#include <string>

class Object;

class FileManager
{
public:
	static void WriteObjectBinaryFile(Object* object, std::string path);
	static void ReadObjectBinaryFile(std::string path);
};