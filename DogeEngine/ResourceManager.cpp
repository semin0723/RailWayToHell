#include <filesystem>
#include "ResourceManager.h"
#include "ImageManager.h"
#include "defines.h"
#include "Core.h"

#include <thread>
#include <Windows.h>

class Sction
{
public:
	static CRITICAL_SECTION cs;
	Sction() : 
		threadManager([this]()
		{
			while (!isDie)
			{
				for (auto iter = threads.begin(); iter != threads.end(); )
				{
					if (iter->joinable())
					{
						iter->join();
						iter = threads.erase(iter);
					}
					else
					{
						iter++;
					}
				}
				Sleep(1000);
			}
		})
	{
		InitializeCriticalSection(&cs);
		
	}
	~Sction()
	{
		isDie = true;
		DeleteCriticalSection(&cs);
		for (auto& i : threads)
		{
			if (i.joinable())
			{
				i.join();
			}
			else
			{
				i.~thread();
			}
		}
		threadManager.join();
	}
	std::list<std::thread> threads;
	std::thread threadManager;
	bool isDie = false;

};
CRITICAL_SECTION Sction::cs;
static Sction section;
class Lock
{
public:
	Lock()
	{
		EnterCriticalSection(&section.cs);
	}
	~Lock()
	{
		LeaveCriticalSection(&section.cs);
	}

	
};

static ResourceManager* instance;
ResourceManager& ResourceManager::GetInstance()
{
	return *instance;
}

ResourceManager::ResourceManager()
{
	images.reserve(1000);
	imageAddcount.reserve(1000);

	assert(!instance);
	instance = this;
}


// ID2D1Bitmap ��ü�� CoUninitialize() ���Ŀ� �����Ǵ� ������ ����
// �Ҹ��ڷ� ��������� CoUninitialize()���� ���� Release����� �޸� ���� ����
ResourceManager::~ResourceManager()
{
	for (auto image : images)
	{
		image.second.ReleaseAndGetAddressOf();
	}

	// FMOD ��ü�� ���� release�� ����� �Ѵ�. �̷� �Ÿ� shared_ptr�� �� ��?
	for (auto a : audio)
	{
		a.second->release();
		a.second = nullptr;
	}

	instance = nullptr;
}

ComPtr<ID2D1Bitmap> ResourceManager::AddImageByPath(std::string key, std::wstring path, float quality)
{
	Lock lock;
	ResourceManager& instance = GetInstance();

	if (GetInstance().images[key])
	{
		GetInstance().imageAddcount[key]++;

		return GetInstance().images[key];
	}

	std::filesystem::path p(path.c_str());
	Assert(std::filesystem::exists(p) != false, "�������� �ʴ� ������ ���ҽ��� �߰��մϴ�.");

	ComPtr<ID2D1Bitmap> bitmap = DG::ImageManager::GetD2DBitmapFromFile(path, quality);

	instance.images[key] = bitmap;
	GetInstance().imageAddcount[key]++;

	return bitmap;
}


void ResourceManager::AddImageByPathThread(std::string key, std::wstring path, float quality)
{
	Lock lock;
	if (GetInstance().imageAddcount[key])
	{
		GetInstance().imageAddcount[key]++;
		return;
	}
	section.threads.emplace_back([key, path, quality]()
								 {


									 std::filesystem::path p(path.c_str());
									 Assert(std::filesystem::exists(p) != false, "�������� �ʴ� ������ ���ҽ��� �߰��մϴ�.");

									 ComPtr<ID2D1Bitmap> bitmap = DG::ImageManager::GetD2DBitmapFromFile(path, quality);
									 
									 Lock lock;

									 GetInstance().images[key] = bitmap;
									 ++GetInstance().imageAddcount[key];
								 });
}

ComPtr<ID2D1Bitmap> ResourceManager::GetImage(std::string key)
{

	ResourceManager& instance = GetInstance();

	if (instance.images.find(key) != instance.images.end())
	{
		return instance.images[key];
	}
	else
	{
		//printf("���ҽ� �Ŵ������� �̹����� ã�� �� ����: %s\n", key.c_str());
		//Assert(false, "�������� �ʴ� Ű");
		return nullptr;
	}
}

void ResourceManager::DeleteImage(std::string key)
{

	Lock lock;
	if (auto countiter = GetInstance().imageAddcount.find(key); countiter != GetInstance().imageAddcount.end())
	{
		auto imageiter = GetInstance().images.find(key);

		int count = --(countiter->second);
		if (0 == count)
		{
			GetInstance().imageAddcount.erase(countiter);

			imageiter->second.ReleaseAndGetAddressOf();
			GetInstance().images.erase(imageiter);
		}
		else if (0 > count)
		{
			section.threads.emplace_back([key]()
										 {
											 auto countiter = GetInstance().imageAddcount.end();
											 while (true)
											 {
												 {
													 Lock lock;
													 countiter = GetInstance().imageAddcount.find(key);
													 if (countiter == GetInstance().imageAddcount.end()) return;
													 if (0 <= countiter->second)
													 {
														 break;
													 }
												 }
												 Sleep(10);
											 }
											 Lock lock;

											 countiter = GetInstance().imageAddcount.find(key);
											 if (countiter == GetInstance().imageAddcount.end()) return;
											 GetInstance().imageAddcount.erase(countiter);


											 auto imageiter = GetInstance().images.find(key);
											 imageiter->second.ReleaseAndGetAddressOf();
											 GetInstance().images.erase(imageiter);

										 });
		}
	}
	else
	{
		std::cout << key << std::endl;
	}
}

bool ResourceManager::IsLoadComplete()
{
	return section.threads.empty();
}

int ResourceManager::GetLoadThreadCount()
{
	return section.threads.size();
}





std::shared_ptr<AtlasImage> ResourceManager::AddAtlasImageByPath(std::string key, std::wstring path, int columnCount, int rowCount, float quality)
{
	ResourceManager& instance = GetInstance();

	Assert(instance.atlases.find(key) == instance.atlases.end(), "�ߺ��� Ű�� ���� ���ҽ��� �߰��մϴ�.");

	std::filesystem::path p(path.c_str());
	Assert(std::filesystem::exists(p) != false, "�������� �ʴ� ������ ���ҽ��� �߰��մϴ�.");

	std::shared_ptr<AtlasImage> atlas(new AtlasImage());
	atlas.get()->image = DG::ImageManager::GetD2DBitmapFromFile(path, quality);

	SizeF imageSize = atlas.get()->image.Get()->GetSize();
	float unitWidth = imageSize.width / columnCount;
	float unitHeight = imageSize.height / rowCount;

	for (int row = 0; row < rowCount; ++row)
	{
		for (int col = 0; col < columnCount; ++col)
		{
			RectF rect;
			rect.left = (float)col * unitWidth;
			rect.top = (float)row * unitHeight;
			rect.right = rect.left + unitWidth;
			rect.bottom = rect.top + unitHeight;
			atlas.get()->rects.push_back(rect);
		}
	}

	instance.atlases[key] = atlas;

	return atlas;
}

std::shared_ptr<AtlasImage> ResourceManager::GetAtlasImage(std::string key)
{
	ResourceManager& instance = GetInstance();

	if (instance.atlases.find(key) != instance.atlases.end())
	{
		return instance.atlases[key];
	}
	else
	{
		printf("���ҽ� �Ŵ������� �̹����� ã�� �� ����: %s\n", key.c_str());
		Assert(false, "�������� �ʴ� Ű");
		return nullptr;
	}
}

void ResourceManager::DeleteAtlasImage(std::string key)
{
	ResourceManager& instance = GetInstance();

	if (instance.atlases.find(key) != instance.atlases.end())
	{
		//instance.atlases[key].reset();
		instance.atlases.erase(key);
	}
}

std::vector<ComPtr<ID2D1Bitmap>> ResourceManager::AddGIFImageByPath(std::string key, std::wstring path)
{
	ResourceManager& instance = GetInstance();

	Assert(instance.gifs.find(key) == instance.gifs.end(), "�ߺ��� Ű�� ���� ���ҽ��� �߰��մϴ�.");

	std::filesystem::path p(path.c_str());
	Assert(std::filesystem::exists(p) != false, "�������� �ʴ� ������ ���ҽ��� �߰��մϴ�.");

	std::vector<ComPtr<ID2D1Bitmap>> imgs = DG::ImageManager::GetD2DBitmapsFromGIFFile(path);
	instance.gifs[key] = imgs;

	return imgs;
}

std::vector<ComPtr<ID2D1Bitmap>> ResourceManager::GetGIFImage(std::string key)
{
	ResourceManager& instance = GetInstance();

	if (instance.gifs.find(key) != instance.gifs.end())
	{
		return instance.gifs[key];
	}
	else
	{
		printf("���ҽ� �Ŵ������� �̹����� ã�� �� ����: %s\n", key.c_str());
		Assert(false, "�������� �ʴ� Ű");
		return {};
	}
}

void ResourceManager::DeleteGIFImage(std::string key)
{
	ResourceManager& instance = GetInstance();

	if (instance.gifs.find(key) != instance.gifs.end())
	{
		for (auto iter = instance.gifs[key].begin(); iter != instance.gifs[key].end(); ++iter)
		{
			(*iter).ReleaseAndGetAddressOf();
		}
		instance.gifs.erase(key);
	}
}

CustomFont* ResourceManager::AddFontByPath(std::string key, std::wstring path)
{
	ResourceManager& instance = GetInstance();

	Assert(instance.fonts.find(key) == instance.fonts.end(), "�ߺ��� Ű�� ���� ���ҽ��� �߰��մϴ�.");

	std::filesystem::path p(path.c_str());
	Assert(std::filesystem::exists(p) != false, "�������� �ʴ� ������ ���ҽ��� �߰��մϴ�.");

	ComPtr<IDWriteFontSetBuilder> fontSetBuilder;
	DG::Core::GetInstance().dwFactory->CreateFontSetBuilder(&fontSetBuilder);

	ComPtr<IDWriteFontFile> fontFile;
	ThrowIfFailed(
		DG::Core::GetInstance().dwFactory->CreateFontFileReference(path.c_str(), nullptr, &fontFile)
	);

	BOOL isSupported;
	DWRITE_FONT_FILE_TYPE fileType;
	UINT32 numberOfFonts;
	ThrowIfFailed(
		fontFile->Analyze(&isSupported, &fileType, nullptr, &numberOfFonts)
	);
	if (!isSupported)
	{
		Assert(false, "failed to create font file");
		return nullptr;
	}

	if (numberOfFonts == 0)
	{
		Assert(false, "failed to find font");
		return nullptr;
	}
	else if (numberOfFonts != 1)
	{
		Assert(false, "DogeEngine does not support adding multi-font collection");
		return nullptr;
	}



	// �´� ���� �𸣰ڴµ� ���� font set�� ����� ���� face index(�Ƹ� �ϳ��� ��Ʈ �ȿ� ���� ������ ������ ������ �� �ε����� ���ϴ� ��. ������ ���� ��Ʈ�� �ִ� ��Ʈ �ݷ����� �߰��� �����ϹǷ� 0���� �����س���)�� ������ font face reference��� ���� �������� font set, font collection�� ���� �� �ֳ�����.
	ComPtr<IDWriteFontFaceReference> fontFaceReference;
	ThrowIfFailed(
		DG::Core::GetInstance().dwFactory->CreateFontFaceReference(fontFile.Get(), 0, DWRITE_FONT_SIMULATIONS_NONE, &fontFaceReference)
	);
	ThrowIfFailed(
		fontSetBuilder->AddFontFaceReference(fontFaceReference.Get())
	);


	// font set, font collection ����
	ComPtr<IDWriteFontSet> fontSet;
	ThrowIfFailed(
		fontSetBuilder->CreateFontSet(&fontSet)
	);
	ComPtr<IDWriteFontCollection1> fontCollection;
	ThrowIfFailed(
		DG::Core::GetInstance().dwFactory->CreateFontCollectionFromFontSet(fontSet.Get(), fontCollection.GetAddressOf())
	);

	
	// ��Ʈ �̸� ���ϱ�
	ComPtr<IDWriteFontFace3> fontFace;
	fontFaceReference->CreateFontFace(&fontFace);
	DWRITE_FONT_FACE_TYPE fontFaceType = fontFace->GetType();
	if (fontFaceType != DWRITE_FONT_FACE_TYPE_TRUETYPE && fontFaceType != DWRITE_FONT_FACE_TYPE_TRUETYPE_COLLECTION)
	{
		Assert(false, "failed to find true font type");
		return nullptr;
	}

	ComPtr<IDWriteLocalizedStrings> fontFamilyName;
	fontFace->GetFamilyNames(&fontFamilyName);
	UINT32 index = 0;
	BOOL exists = FALSE;
	fontFamilyName->FindLocaleName(L"ko-KR", &index, &exists);
	if (exists == FALSE)
	{
		// ko-KR ���� �̸��� ã�� �� ���� ��� en-us(0��)�� ���
		index = 0;
	}
	UINT32 length = 0;
	fontFamilyName->GetStringLength(index, &length);
	wchar_t* fontName = new wchar_t[length + 1];
	fontFamilyName->GetString(index, fontName, length + 1);



	// font set, font collection, font name�� �����صΰ� �������� �Ҹ���ѵ� ��
	instance.fonts[key] = CustomFont{};
	instance.fonts[key].fontSet = fontSet;
	instance.fonts[key].fontCollection = fontCollection;
	instance.fonts[key].fontName = std::wstring(fontName);
	delete[] fontName;


	return &instance.fonts[key];
}

CustomFont* ResourceManager::GetFont(std::string key)
{
	ResourceManager& instance = GetInstance();

	if (instance.fonts.find(key) != instance.fonts.end())
	{
		return &instance.fonts[key];
	}
	else
	{
		printf("���ҽ� �Ŵ������� ��Ʈ�� ã�� �� ����: %s\n", key.c_str());
		Assert(false, "�������� �ʴ� Ű");
		return nullptr;
	}
}

void ResourceManager::DeleteFont(std::string key)
{
	ResourceManager& instance = GetInstance();

	if (instance.fonts.find(key) != instance.fonts.end())
	{
		auto p = instance.fonts[key];
		p.fontCollection.ReleaseAndGetAddressOf();
		p.fontSet.ReleaseAndGetAddressOf();
		instance.fonts.erase(key);
	}
}

std::shared_ptr<Animation> ResourceManager::AddAnimation(std::string key)
{
	ResourceManager& instance = GetInstance();

	Assert(instance.animations.find(key) == instance.animations.end(), "�ߺ��� Ű�� ���� ���ҽ��� �߰��մϴ�.");

	std::shared_ptr<Animation> anim = std::make_shared<Animation>();

	instance.animations[key] = anim;

	return anim;
}

std::shared_ptr<Animation> ResourceManager::GetAnimation(std::string key)
{
	ResourceManager& instance = GetInstance();

	if (instance.animations.find(key) != instance.animations.end())
	{
		return instance.animations[key];
	}
	else
	{
		printf("���ҽ� �Ŵ������� �̹����� ã�� �� ����: %s\n", key.c_str());
		Assert(false, "�������� �ʴ� Ű");
		return nullptr;
	}
}

void ResourceManager::DeleteAnimation(std::string key)
{
	ResourceManager& instance = GetInstance();

	if (instance.animations.find(key) != instance.animations.end())
	{
		instance.animations.erase(key);
	}
}

NineSlicedImage* ResourceManager::AddNineSlicedImageByPath(std::string key, std::wstring path, int leftGap, int rightGap, int topGap, int bottomGap, float quality)
{
	ResourceManager& instance = GetInstance();

	Assert(instance.nineSlicedImage.find(key) == instance.nineSlicedImage.end(), "�ߺ��� Ű�� ���� ���ҽ��� �߰��մϴ�.");

	std::filesystem::path p(path.c_str());
	Assert(std::filesystem::exists(p) != false, "�������� �ʴ� ������ ���ҽ��� �߰��մϴ�.");

	instance.nineSlicedImage[key] = NineSlicedImage{};
	instance.nineSlicedImage[key].image = DG::ImageManager::GetD2DBitmapFromFile(path, quality);
	instance.nineSlicedImage[key].leftGap = leftGap;
	instance.nineSlicedImage[key].rightGap = rightGap;
	instance.nineSlicedImage[key].topGap = topGap;
	instance.nineSlicedImage[key].bottomGap = bottomGap;

	return &instance.nineSlicedImage[key];
}
NineSlicedImage* ResourceManager::GetNineSlicedImage(std::string key)
{
	ResourceManager& instance = GetInstance();

	if (instance.nineSlicedImage.find(key) != instance.nineSlicedImage.end())
	{
		return &instance.nineSlicedImage[key];
	}
	else
	{
		printf("���ҽ� �Ŵ������� �̹����� ã�� �� ����: %s\n", key.c_str());
		Assert(false, "�������� �ʴ� Ű");
		return nullptr;
	}
}


FMOD::Sound* ResourceManager::AddAudioByPath(std::string key, std::wstring path)
{
	Assert(GetInstance().audio.find(key) == GetInstance().audio.end(), "�ߺ��� Ű�� ���� ���ҽ��� �߰��մϴ�.");

	std::filesystem::path p(path.c_str());
	Assert(std::filesystem::exists(p) != false, "�������� �ʴ� ������ ���ҽ��� �߰��մϴ�.");

	std::string _path;
	_path.assign(path.begin(), path.end());

	FMOD::Sound* sound = nullptr;

	FMOD_RESULT fmod_result;
	fmod_result = DG::Core::GetInstance().fmod_system->createSound(_path.c_str(), FMOD_DEFAULT, 0, &sound);

	GetInstance().audio[key] = sound;

	return sound;
}
FMOD::Sound* ResourceManager::GetAudio(std::string key)
{
	ResourceManager& instance = GetInstance();

	if (instance.audio.find(key) != instance.audio.end())
	{
		return instance.audio[key];
	}
	else
	{
		Assert(false, "�������� �ʴ� Ű");
		return nullptr;
	}
}
void ResourceManager::DeleteAudio(std::string key)
{
	ResourceManager& instance = GetInstance();

	if (instance.audio.find(key) != instance.audio.end())
	{
		instance.audio[key]->release();
		instance.audio.erase(key);
	}
}

FmodSoundBank* ResourceManager::AddSoundBankByPath(std::string key, std::wstring path, std::wstring subFilepath)
{
	std::string _path;
	_path.assign(path.begin(), path.end());
	
	FMOD::Sound* newStream = NULL;
	FMOD_RESULT result = DG::Core::GetInstance().fmod_system->createSound(_path.c_str(), FMOD_DEFAULT | FMOD_2D, nullptr, &newStream);
	GetInstance().soundBank[key] = new FmodSoundBank(newStream, subFilepath);
	return GetInstance().soundBank[key];
}

FmodSoundBank* ResourceManager::GetSoundBank(std::string key)
{
	ResourceManager& instance = GetInstance();

	if (instance.soundBank.find(key) != instance.soundBank.end())
	{
		return instance.soundBank[key];
	}
	else
	{
		Assert(false, "�������� �ʴ� Ű");
		return nullptr;
	}
}

void ResourceManager::RemoveSoundBankByPath(std::string key)
{
	ResourceManager& instance = GetInstance();

	if (instance.soundBank.find(key) != instance.soundBank.end())
	{
		instance.soundBank.erase(key);
	}
}


SoundBank* ResourceManager::AddAudioBankByPath(std::string key, std::wstring path)
{
	std::string _path;
	_path.assign(path.begin(), path.end());

	FMOD::Studio::Bank* newBank = NULL;
	FMOD_RESULT result = DG::Core::GetInstance().fmod_Studiosystem->loadBankFile(_path.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &newBank);
	GetInstance().banks[key] = new SoundBank(newBank);

	return GetInstance().banks[key];
}

SoundBank* ResourceManager::GetAudioBank(std::string key)
{
	ResourceManager& instance = GetInstance();

	if (instance.banks.find(key) != instance.banks.end())
	{
		return instance.banks[key];
	}
	else
	{
		Assert(false, "�������� �ʴ� Ű");
		return nullptr;
	}
}

void ResourceManager::DeleteAudioBank(std::string key)
{
	ResourceManager& instance = GetInstance();

	if (instance.banks.find(key) != instance.banks.end())
	{
		instance.banks.erase(key);
	}
}

SoundBankChannel* SoundBank::Play(const char* eventName)
{
	FMOD_RESULT result;

	FMOD::Studio::EventDescription* findEvent = nullptr;
	FMOD::Studio::EventDescription* temp[22];
	int tempp;
	result = bank->getEventList(temp, 22, &tempp);
	result = DG::Core::GetInstance().GetFmodStudioSystem()->getEvent(eventName, &findEvent);
	if (!findEvent)
	{
		return nullptr;
	}
	eventDescription = findEvent;

	FMOD::Studio::EventInstance* eventInstance = nullptr;
	result = findEvent->createInstance(&eventInstance);

	result = eventInstance->start();
	return new SoundBankChannel(eventInstance);
}