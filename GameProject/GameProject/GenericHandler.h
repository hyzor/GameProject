#ifndef GENERICHANDLER_H
#define GENERICHANDLER_H
#include "GenericModel.h"
#include "GenericSkinnedModel.h"
#include <map>

/*
Singleton med alla Generic(Model/SkinnedModel)-object i, dessa läggs i en associativ array
vid kompilering, för att sedan få access till dessa behöver man bara göra en
pekare och kalla på rätt funktion med rätt key
*/

class GenericHandler
{
public:
	static GenericHandler* GetInstance()
	{
		static GenericHandler instance;
		return &instance;
	}
	~GenericHandler();
	void Initialize(ID3D11Device* device, TextureManager* mTextureMgr);
	GenericModel* GetGenericModel(std::string key);
	GenericSkinnedModel* GetGenericSkinnedModel(std::string key);

private:
	GenericHandler();
	GenericHandler(GenericHandler const&) {}
	void operator=(GenericHandler const&) {}

	std::map<std::string, GenericModel*> mGModels;
	std::map<std::string, GenericSkinnedModel*> mGSModels;
};
#endif