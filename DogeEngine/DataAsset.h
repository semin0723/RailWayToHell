#pragma once


#include <map>
#include <string>
#include <fstream>
#include "ReflectObject.h"

class DataAsset : public ReflectObject
{
public:
	virtual void DeSerialize(const Archive& archive);

	template<class T = DataAsset>
	static void LoadDataAsset(const char* fileName, std::vector<T>& dataArray)
	{
		std::vector<std::string> memberNames;
		dataArray.resize(0);

		std::ifstream fileContents(fileName);

		if (fileContents.is_open())
		{
			std::string line;


			std::string cell;
			std::vector<std::string> row;

			std::getline(fileContents, line);
			std::stringstream lineStream(line);

			while (std::getline(lineStream, cell, ','))
			{
				row.push_back(cell);
			}
			memberNames = std::move(row);

			Archive tempArchive;
			while (std::getline(fileContents, line, '\n'))
			{
				row.clear();
				std::stringstream lineStream(line);
				while (std::getline(lineStream, cell, ','))
				{
					row.push_back(cell);
				}
				bool isEmpty = true;
				for (size_t i = 0; i < row.size(); i++)
				{
					if (!row[i].empty())
					{
						isEmpty = false;
					}

				}
				if (isEmpty) continue;
				auto rowIter = row.begin();
				for (auto memberiter = memberNames.begin(); memberiter != memberNames.end(); memberiter++)
				{
					if (size_t findPos = (*rowIter).find("\"\"\""); findPos != std::string::npos)
					{
						std::string str = "";
						str += (*rowIter);
						size_t findPos2 = (*rowIter).rfind("\"\"\"");
						while (!(findPos2 != std::string::npos && findPos != findPos2))
						{
							str += ',';
							str += *++rowIter;
							findPos2 = (*rowIter).rfind("\"\"\"");
						}

						++rowIter;
						
						std::string tempstr = str.substr(findPos + 3, str.size() - 6);
						tempArchive[*memberiter] = str.substr(findPos + 3, str.size() - 6);
					}
					else if (size_t findPos = (*rowIter).find('\"'); findPos != std::string::npos)
					{
						std::string inputstring;
						YAML::Emitter emitter;
						emitter << YAML::BeginSeq;
						inputstring.assign(*rowIter, findPos + 1, rowIter->size());
						emitter << inputstring;
						++rowIter;
						while (true)
						{
							emitter << *rowIter;
							if (size_t findPos2 = (*++rowIter).find('\"'); findPos2 != std::string::npos)
							{
								inputstring.assign(*rowIter, 0, findPos2);
								emitter << inputstring;
								++rowIter;
								break;
							}
						}

						emitter << YAML::EndSeq;
						tempArchive[*memberiter] = LoadArchive(emitter.c_str());
					}
					else
					{
						tempArchive[*memberiter] = *rowIter;
						++rowIter;
					}
				}
				std::stringstream ss;
				ss << tempArchive;
				auto temppp = ss.str();
				dataArray.emplace_back().DeSerialize(tempArchive);
			}
		}
	}

#define DataAssetReflect(x)	\

	GENERATED_BODY(DataAsset, ReflectObject)
};
