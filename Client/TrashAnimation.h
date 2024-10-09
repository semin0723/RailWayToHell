#pragma once



class TrashAnimation : public Object
{
public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void OnDestroy() override;

	void UnLoadSprite();
	void LoadSprite();
	void NextSprite();

	bool IsMaxIndex();
	bool IsLoadMaxIndex();
	bool IsUnLoadMaxIndex();

	int NextIndex();
	int NextLoadIndex();
	int NextUnLoadIndex();
	void InitIndex();

public:
	SpriteRenderer* sprite;

public:
	std::wstring path;
	std::string key;
	std::string pluskey;
	float totalTime;
	bool isloop;
	bool isReverse;
	float quality;

private:
	int index;
	int unloadIndex;
	int loadIndex;
	int maxIndex;
	float elapsedTime;
};


