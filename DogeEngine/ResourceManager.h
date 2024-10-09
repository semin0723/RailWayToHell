#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include "AudioFramework.h"
#include "DirectXFramework.h"
#include "AtlasImage.h"
#include "CustomFont.h"
#include "Animation.h"
#include "NineSlicedImage.h"

namespace DG
{
	class Core;
}

class ResourceManager
{
	std::unordered_map<std::string, ComPtr<ID2D1Bitmap>> images;
	std::unordered_map<std::string, int> imageAddcount;
	std::unordered_map<std::string, std::shared_ptr<AtlasImage>> atlases;
	std::unordered_map<std::string, std::vector<ComPtr<ID2D1Bitmap>>> gifs;
	std::unordered_map<std::string, CustomFont> fonts;
	std::unordered_map<std::string, NineSlicedImage> nineSlicedImage;
	std::unordered_map<std::string, std::shared_ptr<Animation>> animations;

	// �׷� �ʿ�� ������ FMOD::Sound ��ü�� shared_ptr�� �μ� �����ϴϱ� ResourceManager�� �Ҹ��� �� ������ ����. ������ �𸣰���.
	std::unordered_map<std::string, FMOD::Sound*> audio;
	std::unordered_map<std::string, FmodSoundBank*> soundBank;
	std::unordered_map<std::string, class SoundBank*> banks;

	static ResourceManager& GetInstance();
	ResourceManager();
	~ResourceManager();
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager(ResourceManager&&) = delete;
	friend DG::Core;
public:

	static ComPtr<ID2D1Bitmap> AddImageByPath(std::string key, std::wstring path, float quality = 1.0f);
	static void AddImageByPathThread(std::string key, std::wstring path, float quality = 1.0f);
	static ComPtr<ID2D1Bitmap> GetImage(std::string key);
	static void DeleteImage(std::string key);
	static bool IsLoadComplete();
	static int GetLoadThreadCount();


	static std::shared_ptr<AtlasImage> AddAtlasImageByPath(std::string key, std::wstring path, int columnCount, int rowCount, float quality = 1.0f);
	static std::shared_ptr<AtlasImage> GetAtlasImage(std::string key);
	static void DeleteAtlasImage(std::string key);


	static std::vector<ComPtr<ID2D1Bitmap>> AddGIFImageByPath(std::string key, std::wstring path);
	static std::vector<ComPtr<ID2D1Bitmap>> GetGIFImage(std::string key);
	static void DeleteGIFImage(std::string key);


	static CustomFont* AddFontByPath(std::string key, std::wstring path);
	static CustomFont* GetFont(std::string key);
	static void DeleteFont(std::string key);


	static std::shared_ptr<Animation> AddAnimation(std::string key);
	static std::shared_ptr<Animation> GetAnimation(std::string key);
	static void DeleteAnimation(std::string key);

	// �ӽ�
	static NineSlicedImage* AddNineSlicedImageByPath(std::string key, std::wstring path, int leftGap, int rightGap, int topGap, int bottomGap, float quality = 1.0f);
	static NineSlicedImage* GetNineSlicedImage(std::string key);



	// ������� ��η� �����ϸ� �ߺ��� FMOD::Sound ��ü�� �����ؾ� �ϱ� ������ Sound ��ü�� ������.
	static FMOD::Sound* AddAudioByPath(std::string key, std::wstring path);
	static FMOD::Sound* GetAudio(std::string key);
	static void DeleteAudio(std::string key);


	static FmodSoundBank* AddSoundBankByPath(std::string key, std::wstring path, std::wstring subFilepath);
	static FmodSoundBank* GetSoundBank(std::string key);
	static void RemoveSoundBankByPath(std::string key);



	static SoundBank* AddAudioBankByPath(std::string key, std::wstring path);
	static SoundBank* GetAudioBank(std::string key);
	static void DeleteAudioBank(std::string key);
};
