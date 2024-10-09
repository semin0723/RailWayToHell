#pragma once
#include "Engine.h"

class Dialog : public Object
{
public:
	Dialog() {}
	~Dialog() {}

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

	void SetText(std::wstring text, int fontSize);
	void SetText(std::wstring text, int fontSize, Vector3 offset);

	Object* textObject;
	SpriteRenderer* spriteRenderer;
private:

};

