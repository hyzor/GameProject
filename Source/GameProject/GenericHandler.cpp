#include "GenericHandler.h"

GenericHandler* GenericHandler::mInstance = 0;

GenericHandler* GenericHandler::GetInstance()
{
	if(!mInstance)
		mInstance = new GenericHandler();
	return mInstance;
}

GenericHandler::GenericHandler()
{
	this->mGModels = std::map<std::string, GenericModel*>();
	this->mGSModels = std::map<std::string, GenericSkinnedModel*>();
}

GenericHandler::~GenericHandler()
{
}

void GenericHandler::Initialize(ID3D11Device* device, TextureManager* mTextureMgr)
{
	// Initializing GenericModel objects
	//mGModels["Duck"] = new GenericModel(device, mTextureMgr, "Data\\Models\\Collada\\duck.obj", L"Data\\Models\\Collada\\");
	//mGModels["Player"] = new GenericModel(device, mTextureMgr, "Data\\Models\\OBJ\\Cop\\cop.obj", L"Data\\Models\\OBJ\\Cop\\");

	mGModels["Switch"] = new GenericModel(device, mTextureMgr, "Data\\Models\\Collada\\Switch.obj", L"Data\\Models\\Collada\\");

	mGModels["Platform1"] = new GenericModel(device, mTextureMgr, "Data\\Models\\Collada\\Platform1\\Platform1.obj", L"Data\\Models\\Collada\\Platform1\\");
	mGModels["Platform2"] = new GenericModel(device, mTextureMgr, "Data\\Models\\Collada\\Platform2\\Platform2.obj", L"Data\\Models\\Collada\\Platform2\\");
	mGModels["Platform3"] = new GenericModel(device, mTextureMgr, "Data\\Models\\Collada\\Platform3\\Platform3.obj", L"Data\\Models\\Collada\\Platform3\\");
	mGModels["Platform4"] = new GenericModel(device, mTextureMgr, "Data\\Models\\Collada\\Platform4\\Platform4.obj", L"Data\\Models\\Collada\\Platform4\\");

	mGModels["Gun"] = new GenericModel(device, mTextureMgr, "Data\\Models\\Collada\\Gun\\Gun.obj", L"Data\\Models\\Collada\\Gun\\");

	// Initializing GenericSkinnedModel objects
	mGSModels["SkinnedModel"] = new GenericSkinnedModel(device, *mTextureMgr, "Data\\Models\\Collada\\AnimTest\\test_Collada_DAE.DAE", L"Data\\Models\\Collada\\AnimTest\\");
}

GenericModel* GenericHandler::GetGenericModel(std::string key)
{
	if(mGModels[key])
		return mGModels[key];
	return nullptr;
}

GenericSkinnedModel* GenericHandler::GetGenericSkinnedModel(std::string key)
{
	if(mGSModels[key])
		return mGSModels[key];
	return nullptr;
}

void GenericHandler::Shutdown()
{
	for(auto& it(mGModels.begin()); it != mGModels.end(); ++it)
	{
		if(it->second)
			SafeDelete(it->second);
	}
	mGModels.clear();

	for(auto& it(mGSModels.begin()); it != mGSModels.end(); ++it)
	{
		if(it->second)
			SafeDelete(it->second);
	}
	mGSModels.clear();

	if(mInstance)
		SafeDelete(mInstance);
}