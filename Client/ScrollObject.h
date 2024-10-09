#pragma once

class ScrollObject : public Object
{
public:
	void Awake() override;
	void Update() override;
	void ImageChange(std::string imageName);

public:
	float speed;

private:
	Object* sprite1;
	Object* sprite2;
	std::string imageName;
};

class ScrollBackGround : public Object
{
public:
	void Awake() override;
	void SetBackGround(std::string backGroundName);

private:
	ScrollObject* BackGround5;
	ScrollObject* BackGround4;
	ScrollObject* BackGround3;
	ScrollObject* BackGround2;
	ScrollObject* BackGround1;
};
