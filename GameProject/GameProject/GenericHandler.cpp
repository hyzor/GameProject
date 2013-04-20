#include "GenericHandler.h"

GenericHandler::GenericHandler()
{
	this->mGModels = std::map<std::string, GenericModel*>();
	this->mGSModels = std::map<std::string, GenericSkinnedModel*>();
}

GenericHandler::~GenericHandler()
{
#ifndef _DEBUG | DEBUG
	std::map<std::string, GenericModel*>::iterator GenericCurr, GenericEnd;
	for(GenericCurr = mGModels.begin(), GenericEnd = mGModels.end(); GenericCurr != GenericEnd; GenericCurr++)
	{
		SafeDelete(GenericCurr->second);
	}

	std::map<std::string, GenericSkinnedModel*>::iterator SkinnedCurr, SkinnedEnd;
	for(SkinnedCurr = mGSModels.begin(), SkinnedEnd = mGSModels.end(); SkinnedCurr != SkinnedEnd; SkinnedCurr++)
	{
		SafeDelete(SkinnedCurr->second);
	}
#endif
}

void GenericHandler::Initialize(ID3D11Device* device, TextureManager* mTextureMgr)
{
	// Initializing GenericModel objects
	mGModels["Duck"] = new GenericModel(device, mTextureMgr, "Data\\Models\\Collada\\duck.obj", L"Data\\Models\\Collada\\");
	mGModels["Player"] = new GenericModel(device, mTextureMgr, "Data\\Models\\OBJ\\Cop\\cop.obj", L"Data\\Models\\OBJ\\Cop\\");

	// Initializing GenericSkinnedModel objects
	mGSModels["SkinnedModel"] = new GenericSkinnedModel(device, *mTextureMgr, "Data\\Models\\Collada\\AnimTest\\test_Collada_DAE.DAE", L"Data\\Models\\Collada\\AnimTest\\");
}

GenericModel* GenericHandler::GetGenericModel(std::string key)
{
	return mGModels[key];
}

GenericSkinnedModel* GenericHandler::GetGenericSkinnedModel(std::string key)
{
	return mGSModels[key];
}
