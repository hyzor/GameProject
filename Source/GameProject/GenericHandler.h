#ifndef GENERICHANDLER_H
#define GENERICHANDLER_H
#include "GenericModel.h"
#include "GenericSkinnedModel.h"
#include "CollisionModel.h"
#include <map>

/*
Singleton med alla Generic(Model/SkinnedModel)-object i, dessa läggs i en associativ array
vid kompilering, för att sedan få access till dessa behöver man bara göra en
pekare och kalla på rätt funktion med rätt key
*/

class GenericHandler
{
public:
	static GenericHandler* GetInstance();
	~GenericHandler();
	void Initialize(ID3D11Device* device, TextureManager* mTextureMgr);
	GenericModel* GetGenericModel(std::string key);
	GenericSkinnedModel* GetGenericSkinnedModel(std::string key);
	CollisionModel* GetCollisionModel(std::string key);

	void Shutdown();

private:
	static GenericHandler* mInstance;
	GenericHandler();
	GenericHandler(GenericHandler const&) {}
	void operator=(GenericHandler const&) {}

	std::map<std::string, GenericModel*> mGModels;
	std::map<std::string, GenericSkinnedModel*> mGSModels;
	std::map<std::string, CollisionModel*> cModels;
};
#endif