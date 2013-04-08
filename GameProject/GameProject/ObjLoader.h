//----------------------------------------------------------------------------------------
// File: ObjLoader.cpp
//
// This class is used for loading all the mesh data from an .obj file
// and materials from an .mtl file
//----------------------------------------------------------------------------------------


#ifndef OBJLOADER_H_
#define OBJLOADER_H_

#include <d3dUtilities.h>

// User defined
#include "Vertex.h"
#include "MeshGeometry.h"

struct ObjMaterial
{
	// Standard material properties as defined in LightDef.h
	// Ka = Ambient
	// Kd = Diffuse
	// Ks = Specular
	Material mat;

	int shininess;					// Ns
	float alpha;					// d or Tr
	bool useSpecular;				// illum (if (illum == 2) is true)
	float refractiveIndex;			// Ni (also known as optical density)
	std::wstring diffuseMapName;	// map_Kd
	std::string name;				// Defined by newmtl

	// Tf = Transmission filter (R G B) (0.0 = disabled 1.0 = enabled respectively)
	// Tf is not currently used
};

// Used for a hash table vertex cache
struct CacheEntry
{
	UINT index;
	CacheEntry* next;
};

class ObjLoader
{
public:
	ObjLoader(void);
	~ObjLoader(void);

	HRESULT loadObj(
		const std::string& fileName,
		const std::string& folderPath,
		std::vector<Vertex::Basic32>& vertices,
		std::vector<UINT>& indices,
		std::vector<MeshGeometry::Subset>& subsets,
		std::vector<ObjMaterial>& materials);

private:
	bool loadMtl(
		const std::string& fileName,
		const std::string& folderPath,
		std::vector<ObjMaterial>& materials);

	void initMaterial(ObjMaterial* material);

	UINT addVertex(UINT hash, Vertex::Basic32* vertex, std::vector<Vertex::Basic32>& vertices, UINT& vertexCount,
		std::vector<MeshGeometry::Subset>& subsets);

	void deleteCache();

	bool addSubset(const UINT faceCount, UINT& faceStart, const UINT vertexCount,
		UINT& vertexStart, UINT& subsetCount, std::vector<MeshGeometry::Subset>& subsets);

	std::vector<CacheEntry*> mVertexCache;
};

#endif