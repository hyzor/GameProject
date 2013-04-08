//----------------------------------------------------------------------------------------
// File: ObjLoader.cpp
//
// This class is used for loading all the mesh data from an .obj file
// and materials from an .mtl file
//----------------------------------------------------------------------------------------

#include "ObjLoader.h"
#include <log.h>

ObjLoader::ObjLoader(void)
{
}


ObjLoader::~ObjLoader(void)
{
	deleteCache();
}

//===========================================================================
// OBJ loader
//===========================================================================
HRESULT ObjLoader::loadObj(
	const std::string& fileName,
	const std::string& folderPath,
	std::vector<Vertex::Basic32>& vertices,
	std::vector<UINT>& indices,
	std::vector<MeshGeometry::Subset>& subsets,
	std::vector<ObjMaterial>& materials)
{

	printf("===============================\n");
	printf(" OBJ Loader START\n");
	printf("===============================\n");

	std::ifstream fileIn(folderPath + fileName);
	//std::wifstream fileIn(folderPath + fileName);

	if (!fileIn.is_open())
	{
		printf("Failed to open %s%s \n", folderPath.c_str(), fileName.c_str());
		printf("OBJ Loader exit...\n\n");
		return false;
	}

	std::string line; // Line cache
	//std::wstring line;

	std::string mtlFileName = "";

	// Cache
	std::vector<XMFLOAT3> vertexPositions;
	std::vector<XMFLOAT2> vertexTexCoords;
	std::vector<XMFLOAT3> vertexNormals;

	// Subset properties
	//DWORD subsetCount = 0;
	UINT faceCount = 0;
	UINT faceStart = 0;
	UINT vertexCount = 0;
	UINT vertexStart = 0;
	UINT subsetCount = 0;

	// Create one default subset
	subsets.push_back(MeshGeometry::Subset());

	while (true)
	{
		fileIn >> line;

		// End of file
		if (!fileIn)
			break;

		// Vertex position
		if (line.compare("v") == 0)
		{
			float x, y, z;
		    fileIn >> x >> y >> z;
			vertexPositions.push_back(XMFLOAT3(x, y, z));
		}

		// Vertex TexCoord
		else if (line.compare("vt") == 0)
		{
			float u, v;
			fileIn >> u >> v;
			vertexTexCoords.push_back(XMFLOAT2(u, v));
		}

		// Vertex Normal
		else if (line.compare("vn") == 0)
		{
			float x, y, z;
			fileIn >> x >> y >> z;
			vertexNormals.push_back(XMFLOAT3(x, y, z));
		}

		// Face
		else if (line.compare("f") == 0)
		{
			UINT posIndex, texCoordIndex, normalIndex;
			Vertex::Basic32 vertex;

			// Faces are divided by vp1/vtc1/vn1 vp2/vtc2/vn2 vp3/vtc3/vn3
			// So iterate three times
			for (UINT tmpFace = 0; tmpFace < 3; tmpFace++)
			{
				ZeroMemory(&vertex, sizeof(Vertex::Basic32));

				// First read vertex position index
				fileIn >> posIndex;
				vertex.position = vertexPositions.at(posIndex - 1);

				// If things are as they should, encounter first slash
				if ('/' == fileIn.peek())
				{
					fileIn.ignore(); // Ignore first slash

					// Texture coordinate index
					if ('/' != fileIn.peek())
					{
						fileIn >> texCoordIndex;
						vertex.texCoord = vertexTexCoords.at(texCoordIndex - 1);
					}

					// Second slash
					if ('/' == fileIn.peek())
					{
						fileIn.ignore(); // Ignore second slash

						// Vertex normal index
						fileIn >> normalIndex;
						vertex.normal = vertexNormals.at(normalIndex - 1);
					}
				}

				// If a duplicate vertex doesn't exist, add it to vertices array
				UINT index = addVertex(posIndex, &vertex, vertices, vertexCount, subsets);
				if (index == (UINT)-1)
					return E_OUTOFMEMORY;

				// Store index in indices array
				indices.push_back(index);
			}

			// Increase currently used subset's face count
			subsets.back().FaceCount++;

			// Increase overall face count
			faceCount++;
		}

		// Material library (.mtl file)
		else if (line.compare("mtllib") == 0)
		{
			fileIn >> mtlFileName;

			printf("Using material library %s\n\n", mtlFileName.c_str());

			// Load material file
			if (!loadMtl(mtlFileName, folderPath, materials))
				return false;
		}

		// Material
		else if (line.compare("usemtl") == 0)
		{
			std::string mtlName;
			fileIn >> mtlName;

			printf("Searching for material %s%s", mtlName.c_str(), "... ");

			bool foundMaterial = false;

			addSubset(faceCount, faceStart, vertexCount, vertexStart, subsetCount, subsets);

			// Find material in .mtl file
			for (UINT matIndex = 0; matIndex < materials.size(); matIndex++)
			{
				// If we found it
				if (materials.at(matIndex).name.compare(mtlName) == 0)
				{
					printf("Material found!\n");
					foundMaterial = true;					

					break;
				}
			}

			// If we did not find it, create it
			if (!foundMaterial)
			{
				printf("Material not found, creating new\n");

				ObjMaterial objMaterial;
				initMaterial(&objMaterial);
				objMaterial.name = mtlName;
				materials.push_back(objMaterial);
			}
		}

		else
		{

		}

		fileIn.ignore(1000, '\n');
	}

	fileIn.close();
	deleteCache();

	printf("\n--- OBJ Stats ---\n");
	printf("Total vertices read: %d\n", vertices.size());
	printf("Total faces read: %d\n", faceCount);
	printf("Total indices read: %d\n", indices.size());
	printf("Subsets created: %d\n\n", subsets.size());

	for (UINT i = 0; i < subsets.size(); ++i)
	{
		printf("Subset %d\n", subsets.at(i).Id);
		printf("- Faces %d\n", subsets.at(i).FaceCount);
		printf("- Face start %d\n", subsets.at(i).FaceStart);
		printf("- Vertices %d\n", subsets.at(i).VertexCount);
		printf("- Vertex start %d\n", subsets.at(i).VertexStart);
	}

	//printf("----- OBJ Loader exit -----\n");

	printf("===============================\n");
	printf(" OBJ Loader EXIT\n");
	printf("===============================\n\n");

	return true;
}

//=======================================================================================
// MTL loader
//=======================================================================================
bool ObjLoader::loadMtl(
	const std::string& fileName,
	const std::string& folderPath,
	std::vector<ObjMaterial>& materials)
{
	printf("===============================\n");
	printf(" MTL Loader START\n");
	printf("===============================\n");

	std::wifstream fileIn(folderPath + fileName);

	printf("Opening %s\n", fileName.c_str());

	if (!fileIn.is_open())
	{
		printf("Failed to open .mtl file!\n");
		printf("MTL Loader exit...\n\n");
		return false;
	}

	std::wstring line; // Line cache

	int nrOfMaterials = 0;

	// Material cache
	ObjMaterial tmpMaterial;
	initMaterial(&tmpMaterial);

	while(true)
	{

		fileIn >> line; // Read line

		// End of file
		if (!fileIn)
		{
			// If we read at least one material
			if (nrOfMaterials > 0)
			{
				printf("- Creating new material -\n");
				materials.push_back(tmpMaterial); // Push back last material read
			}

			break;
		}

		// New material found
		if (0 == wcscmp(line.c_str(), L"newmtl"))
		{
			// If we already read one or more materials before,
			// insert the last one into materials array before flushing cache
			if (nrOfMaterials > 0)
			{
				printf("- Creating new material -\n");
				materials.push_back(tmpMaterial);
			}

			nrOfMaterials++;

			initMaterial(&tmpMaterial); // Reset material cache values

			std::wstring mtlName;
			fileIn >> mtlName;

			std::string mtlNameStr(mtlName.begin(), mtlName.end());
			tmpMaterial.name = mtlNameStr;

			printf("newmtl %s\n", tmpMaterial.name.c_str());
		}

		// Haven't found a material yet, skip rest
		if (nrOfMaterials == 0)
			continue;

		// Ambient color
		if (0 == wcscmp(line.c_str(), L"Ka"))
		{
			fileIn >> tmpMaterial.mat.Ambient.x >> tmpMaterial.mat.Ambient.y >> tmpMaterial.mat.Ambient.z;

			printf("Ka %f%s%f%s%f\n", tmpMaterial.mat.Ambient.x, " ", tmpMaterial.mat.Ambient.y, " ",
				tmpMaterial.mat.Ambient.z);
		}

		// Diffuse color
		else if (0 == wcscmp(line.c_str(), L"Kd"))
		{
			fileIn >> tmpMaterial.mat.Diffuse.x >> tmpMaterial.mat.Diffuse.y >> tmpMaterial.mat.Diffuse.z;

			printf("Kd %f%s%f%s%f\n", tmpMaterial.mat.Diffuse.x, " ", tmpMaterial.mat.Diffuse.y, " ",
				tmpMaterial.mat.Diffuse.z);
		}

		// Specular color
		else if (0 == wcscmp(line.c_str(), L"Ks"))
		{
			fileIn >> tmpMaterial.mat.Specular.x >> tmpMaterial.mat.Specular.y >> tmpMaterial.mat.Specular.z;

			printf("Ks %f%s%f%s%f\n", tmpMaterial.mat.Specular.x, " ", tmpMaterial.mat.Specular.y, " ",
				tmpMaterial.mat.Specular.z);
		}

		// Alpha
		else if (0 == wcscmp(line.c_str(), L"d") || (0 == wcscmp(line.c_str(), L"Tr")))
		{
			fileIn >> tmpMaterial.alpha;
			printf("d/Tr %f\n", tmpMaterial.alpha);
		}

		// Refractive index
		else if (0 == wcscmp(line.c_str(), L"Ni"))
		{
			fileIn >> tmpMaterial.refractiveIndex;
			printf("Ni %f\n", tmpMaterial.refractiveIndex);
		}

		// Shininess
		else if (0 == wcscmp(line.c_str(), L"Ns"))
		{
			fileIn >> tmpMaterial.shininess;
			printf("Ns %d\n", tmpMaterial.shininess);
		}

		// Specular on/off
		else if (0 == wcscmp(line.c_str(), L"illum"))
		{
			int illumination = 0;
			fileIn >> illumination;
			tmpMaterial.useSpecular = (illumination == 2);

			printf("illum %d\n", illumination);
		}

		// Texture
		else if (0 == wcscmp(line.c_str(), L"map_Kd"))
		{
			std::wstring texName;
			fileIn >> texName;
			//std::string texNameStr(texName.begin(), texName.end());

			tmpMaterial.diffuseMapName = texName;

			printf("map_Kd %ls\n", tmpMaterial.diffuseMapName.c_str());
		}

		else
		{

		}

		fileIn.ignore(1000, L'\n');
	}

	fileIn.close();

	printf("\nMaterials loaded:\n");

	for (UINT i = 0; i < materials.size(); ++i)
	{
		printf("(%d%s", i+1, ") ");
		printf("Name: %s", materials.at(i).name.c_str());
		printf(" Texture: %ls\n", materials.at(i).diffuseMapName.c_str());
	}

	//printf("----- MTL Loader exit -----\n\n");

	printf("===============================\n");
	printf(" MTL Loader EXIT\n");
	printf("===============================\n\n");

	return true;
}

//=======================================================================================
// Function which initializes a material to its default values
//=======================================================================================
void ObjLoader::initMaterial(ObjMaterial* material)
{
	ZeroMemory(material, sizeof(material));

	material->mat.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	material->mat.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material->mat.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 16.0f);
	material->mat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	material->shininess = 0;
	material->alpha = 1.0f;
	material->useSpecular = false;
	material->diffuseMapName = L"";
	material->name = "";
	material->refractiveIndex = 1.0f;
}

//=======================================================================================
// Function which checks for duplicate vertices and handles the vertex adding
//=======================================================================================
UINT ObjLoader::addVertex(UINT hash, Vertex::Basic32* vertex, std::vector<Vertex::Basic32>& vertices,
	UINT& vertexCount, std::vector<MeshGeometry::Subset>& subsets)
{
	bool foundInList = false;
	UINT index = 0;

	// Hash table stores vertex indices according to vertex position's index
	// So if the vertex cache's size is greater than the hash value...
	if ((UINT)mVertexCache.size() > hash)
	{
		// ...set entry to hash index in cache array
		CacheEntry* entry = mVertexCache.at(hash);

		// Traverse the linked list
		while (entry != NULL)
		{
			// Cache the vertex in vertices vector at the "entry index" index
			Vertex::Basic32* cacheVertex = vertices.data() + entry->index;

			// Make a memory comparison of the vertex and the cached one
			// If they are identical, point index buffer to existing vertex
			if (0 == memcmp(vertex, cacheVertex, sizeof(Vertex::Basic32)))
			{
				foundInList = true;
				index = entry->index;
				break;
			}

			entry = entry->next; // Otherwise continue traversing
		}
	}

	// If we did not find the vertex, create a new entry, both within the vertices list
	// and the hash table cache
	if (!foundInList)
	{
		// Add to vertices list
		index = vertices.size();
		vertices.push_back(*vertex);

		// Increase vertex count
		subsets.back().VertexCount++;
		vertexCount++;

		// Add to hash table
		CacheEntry* newEntry = new CacheEntry;
		// Check if out of memory
		if (newEntry == NULL)
			return (UINT)-1;

		newEntry->index = index;
		newEntry->next = NULL;

		// Grow cache if needed until we reach hash value
		while ((UINT)mVertexCache.size() <= hash)
		{
			mVertexCache.push_back(NULL);
		}

		// Add to end of linked list
		CacheEntry* curEntry = mVertexCache.at(hash);

		// If we are at head element
		if (curEntry == NULL)
		{
			mVertexCache.at(hash) = newEntry; // Insert new entry as head value
		}
		// Otherwise find tail...
		else
		{
			while (curEntry->next != NULL)
			{
				curEntry = curEntry->next;
			}

			curEntry->next = newEntry; //... and set our new head value
		}
	}

	return index;
}

void ObjLoader::deleteCache()
{
	// Iterate through cache and subsequent linked lists
	for (UINT i = 0; i < mVertexCache.size(); i++)
	{
		CacheEntry* entry = mVertexCache.at(i);

		while (entry != NULL)
		{
			CacheEntry* next = entry->next;
			SafeDelete(entry);
			entry = next;
		}
	}

	mVertexCache.erase(mVertexCache.begin(), mVertexCache.end());
}

bool ObjLoader::addSubset(const UINT faceCount, UINT& faceStart, const UINT vertexCount,
									UINT& vertexStart, UINT& subsetCount, std::vector<MeshGeometry::Subset>& subsets)
{
	subsetCount++;

	// If we are at first default subset
	if (subsetCount == 1)
	{
		subsets.back().Id = subsetCount - 1;
		subsets.back().FaceStart = faceStart;
		subsets.back().VertexStart = vertexStart;

		return false;
	}

	// Create new subset
	MeshGeometry::Subset subset;
	subset.Id = subsetCount - 1;
	subset.FaceStart = faceStart;
	subset.VertexStart = vertexStart;
	subsets.push_back(subset);

	// Prepare values for next subset
	faceStart = faceCount;
	vertexStart = vertexCount;

	return true;
}
