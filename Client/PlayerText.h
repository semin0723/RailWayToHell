#pragma once
#include "Engine.h"


class PlayerText : public DataAsset
{
public:
	int BubbleID;
	std::string DevComment;
	std::string BubbleCondition;
	std::string BubbleText;
	float BubbleDuration;
	int FontSize;

#define PlayerTextReflect(x)	\
	x(BubbleID)					\
	x(DevComment)					\
	x(BubbleCondition)					\
	x(BubbleText)					\
	x(BubbleDuration)					\
	x(FontSize)					\

	GENERATED_BODY(PlayerText, DataAsset)
};

class PlayerTextManager
{
public:
	static PlayerText& GetText(int textID);
private:
	static PlayerTextManager& GetInstance();
	std::map<int, PlayerText> textMap;
};