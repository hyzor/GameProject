#include "GenericObjectLoader.h"


GenericObjectLoader::GenericObjectLoader(void)
{
}


GenericObjectLoader::~GenericObjectLoader(void)
{
}



//==================================================================================
// Object loader with meshes
//==================================================================================
bool GenericObjectLoader::loadObject(const std::string& fileName,
									 std::vector<GenericMaterial>& materials,
									 std::vector<GenericMesh>& meshes)
{
	using namespace Assimp;

	// Create instance of assimp Importer class
	Importer importer;

	std::string logName = "AssimpMeshLoader.log";

	DefaultLogger::create(logName.c_str(), Logger::VERBOSE);

	bool isTriangulated = true;

	// Read file with post processing flags
	const aiScene* scene = importer.ReadFile(fileName,
		aiProcess_ConvertToLeftHanded | 			// Make compatible with Direct3D
		aiProcessPreset_TargetRealtime_Quality		// Combination of post processing flags
		);										

	// Failed reading file
	if (!scene)
	{
		// Error logging
		DefaultLogger::get()->info("Failed reading file!");
		DefaultLogger::kill();

		return false;
	}

	//----------------------------------------------------
	// Read the scene
	//----------------------------------------------------

	// Check whether or not it has read at least one or more meshes
	// Assimp splits up a mesh if more than one material was found
	// (So it splits up an object into subsets)
	if (scene->HasMeshes())
	{
		UINT totalVertexCount = 0;
		UINT totalFaceCount = 0;

		// Get scene root node
// 		aiNode* rootNode;
// 		rootNode = scene->mRootNode;
// 
// 		aiNode* curNode = rootNode;
// 
// 		// Iterate through node children
// 		for (UINT i = 0; i < curNode->mNumChildren; ++i)
// 		{
// 			std::stringstream sstream;
// 			sstream << "Node: " << curNode->mChildren[i]->mName.C_Str();
// 
// 			// Get childrens mesh indexes
// 			for (UINT j = 0; j < curNode->mChildren[i]->mNumMeshes; ++j)
// 			{
// 				sstream << " (Mesh index: " << curNode->mChildren[i]->mMeshes[j] << ")";
// 				sstream << " (Material index: " << scene->mMeshes[curNode->mChildren[i]->mMeshes[j]]->mMaterialIndex << ")";
// 				sstream << " (Num bones: " << scene->mMeshes[curNode->mChildren[i]->mMeshes[j]]->mNumBones << ")";
// 			}
// 
// 			sstream << " (Num children: " << curNode->mChildren[i]->mNumChildren << ")";
// 
// 			DefaultLogger::get()->info(sstream.str().c_str());
// 		}

		//------------------------------------------------------------
		// Read materials
		//------------------------------------------------------------
		loadMaterials(scene, materials);

		//----------------------------------------------------------
		// Read all scene meshes
		//----------------------------------------------------------
		for (UINT curMesh = 0; curMesh < scene->mNumMeshes; ++curMesh)
		{
			// Get mesh name
			aiMesh* mesh = scene->mMeshes[curMesh];
			aiString meshName = mesh->mName;

			// Number of vertices and faces
			UINT numVertices = mesh->mNumVertices;
			UINT numFaces = mesh->mNumFaces;

			//------------------------------------------------------------
			// Create mesh
			//------------------------------------------------------------
			GenericMesh myMesh;
			//ZeroMemory(&myMesh, sizeof(myMesh));

			//------------------------------------------------------------
			// Read vertices
			//------------------------------------------------------------
			for (UINT j = 0; j < numVertices; ++j)
			{
				Vertex::Basic32 vertex;
				//ZeroMemory(&vertex, sizeof(vertex));

				// Vertex position
				vertex.position.x = mesh->mVertices[j].x;
				vertex.position.y = mesh->mVertices[j].y;
				vertex.position.z = mesh->mVertices[j].z;

				// Vertex normal
				vertex.normal.x = mesh->mNormals[j].x;
				vertex.normal.y = mesh->mNormals[j].y;
				vertex.normal.z = mesh->mNormals[j].z;

				// Vertex texture coordinates (UV)
// 				if (mesh->HasTextureCoords(j))
// 				{
// 					vertex.texCoord.x = mesh->mTextureCoords[0][j].x;
// 					vertex.texCoord.y = mesh->mTextureCoords[0][j].y;
// 				}
// 				else
// 				{
// 					vertex.texCoord.x = 0.0f;
// 					vertex.texCoord.y = 0.0f;
// 				}

				vertex.texCoord.x = mesh->mTextureCoords[0][j].x;
				vertex.texCoord.y = mesh->mTextureCoords[0][j].y;

				// Insert vertex to model
				myMesh.vertices.push_back(vertex);
			}

			//------------------------------------------------------------
			// Read indices
			//------------------------------------------------------------
			// Reminder: AI_SCENE_FLAGS_NON_VERBOSE_FORMAT is set by the
			// aiProcess_JoinIdenticalVertices post processing flag, which
			// means that each face reference a unique set of vertices.
			// Here you could check if AI_SCENE_FLAGS_NON_VERBOSE_FORMAT is
			// true or not, and handle it accordingly.

			for (UINT j = 0; j < numFaces; ++j)
			{
				// If mesh is triangulated
				if (isTriangulated)
				{
					// Always three indices per face
					for (UINT k = 0; k < 3; ++k)
					{
						// Insert indices to model
						myMesh.indices.push_back(mesh->mFaces[j].mIndices[k]);
					}
				}

				// Else find out how many indices there are per face
				else
				{
					for (UINT k = 0; k < mesh->mFaces[j].mNumIndices; ++k)
					{
						// Insert indices to model
						myMesh.indices.push_back(mesh->mFaces[j].mIndices[k]);
					}
				}

			}

			// Update total vertex and face count
			totalVertexCount += numVertices;
			totalFaceCount += numFaces;

			//myMesh.setIndices(device, &myMesh.indices[0], myMesh.indices.size());
			//myMesh.setVertices(device, &myMesh.vertices[0], myMesh.vertices.size());

			myMesh.MaterialIndex = mesh->mMaterialIndex;
			myMesh.FaceCount = numFaces;
			myMesh.VertexCount = numVertices;
			myMesh.Name = materials[myMesh.MaterialIndex].name;

			meshes.push_back(myMesh);

		} // Mesh end

	} // If contains mesh end


	// Clean up after reading file
	DefaultLogger::kill();

	return true;
}


//===================================================================================
// Skinned object loader (objects with bones)
//===================================================================================
bool GenericObjectLoader::loadSkinnedObject(const std::string& fileName,
	std::vector<GenericMaterial>& materials,
	std::vector<GenericSkinnedMesh>& meshes,
	SkinnedData& skinData,
	std::vector<SkinnedDataStructs::Bone>& bones)
{
	using namespace Assimp;

	// Create instance of assimp Importer class
	Importer importer;

	std::string logName = "AssimpMeshLoader.log";

	DefaultLogger::create(logName.c_str(), Logger::VERBOSE);

	bool isTriangulated = true;

	// Read file with post processing flags
	const aiScene* scene = importer.ReadFile(fileName,
		aiProcess_ConvertToLeftHanded | 			// Make compatible with Direct3D
		aiProcessPreset_TargetRealtime_Quality		// Combination of post processing flags
		);										

	// Failed reading file
	if (!scene)
	{
		// Error logging
		DefaultLogger::get()->info("Failed reading file!");
		DefaultLogger::kill();

		return false;
	}

	//----------------------------------------------------
	// Read the scene
	//----------------------------------------------------

	// Check whether or not it has read at least one or more meshes
	// Assimp splits up a mesh if more than one material was found
	// (So it splits up an object into subsets)
	if (scene->HasMeshes())
	{
		UINT totalVertexCount = 0;
		UINT totalFaceCount = 0;

		// Get scene root node
		aiNode* rootNode = scene->mRootNode;

		//std::vector<SkinnedDataStructs::Bone*> globalBoneList;
		//std::vector<Vertex::PosNormalTexTanSkinned*> allVertices;

		std::vector<Vertex::PosNormalTexTanSkinned*> VertexList;

		//------------------------------------------------------------
		// Read materials
		//------------------------------------------------------------
		loadMaterials(scene, materials);

		//----------------------------------------------------------
		// Read all scene meshes
		//----------------------------------------------------------
		for (UINT curMesh = 0; curMesh < scene->mNumMeshes; ++curMesh)
		{
			// Get mesh name
			aiMesh* mesh = scene->mMeshes[curMesh];
			aiString meshName = mesh->mName;

			// Number of vertices and faces
			UINT numVertices = mesh->mNumVertices;
			UINT numFaces = mesh->mNumFaces;

			//------------------------------------------------------------
			// Create mesh
			//------------------------------------------------------------
			GenericSkinnedMesh myMesh;
			//ZeroMemory(&myMesh, sizeof(myMesh));

			//------------------------------------------------------------
			// Read vertices
			//------------------------------------------------------------
			for (UINT j = 0; j < numVertices; ++j)
			{
				Vertex::PosNormalTexTanSkinned vertex;
				//ZeroMemory(&vertex, sizeof(vertex));

				// Vertex position
				vertex.position.x = mesh->mVertices[j].x;
				vertex.position.y = mesh->mVertices[j].y;
				vertex.position.z = mesh->mVertices[j].z;

				// Vertex normal
				vertex.normal.x = mesh->mNormals[j].x;
				vertex.normal.y = mesh->mNormals[j].y;
				vertex.normal.z = mesh->mNormals[j].z;

				// Vertex texture coordinates (UV)
				// 				if (mesh->HasTextureCoords(j))
				// 				{
				// 					vertex.texCoord.x = mesh->mTextureCoords[0][j].x;
				// 					vertex.texCoord.y = mesh->mTextureCoords[0][j].y;
				// 				}
				// 				else
				// 				{
				// 					vertex.texCoord.x = 0.0f;
				// 					vertex.texCoord.y = 0.0f;
				// 				}

				vertex.texCoord.x = mesh->mTextureCoords[0][j].x;
				vertex.texCoord.y = mesh->mTextureCoords[0][j].y;

				vertex.tangentU.x = mesh->mTangents[j].x;
				vertex.tangentU.y = mesh->mTangents[j].y;
				vertex.tangentU.z = mesh->mTangents[j].z;
				vertex.tangentU.w = -1.0f;

				// Set bone indices to default values
				// (Vertex is not affected by any bone yet)
				vertex.boneIndices[0] = 0;
				vertex.boneIndices[1] = 0;
				vertex.boneIndices[2] = 0;
				vertex.boneIndices[3] = 0;

				// No weights yet
				vertex.weights = XMFLOAT3(0.0f, 0.0f, 0.0f);

				// Insert vertex to model
				myMesh.vertices.push_back(vertex);

				VertexList.push_back(&myMesh.vertices.back());

				// Push back the address to our global vertex vector
				//allVertices.push_back(&vertex);
			}

			//------------------------------------------------------------
			// Read indices
			//------------------------------------------------------------
			// Reminder: AI_SCENE_FLAGS_NON_VERBOSE_FORMAT is set by the
			// aiProcess_JoinIdenticalVertices post processing flag, which
			// means that each face reference a unique set of vertices.
			// Here you could check if AI_SCENE_FLAGS_NON_VERBOSE_FORMAT is
			// true or not, and handle it accordingly.

			for (UINT j = 0; j < numFaces; ++j)
			{
				// If mesh is triangulated
				if (isTriangulated)
				{
					// Always three indices per face
					for (UINT k = 0; k < 3; ++k)
					{
						// Insert indices to model
						myMesh.indices.push_back(mesh->mFaces[j].mIndices[k]);
					}
				}

				// Else find out how many indices there are per face
				else
				{
					for (UINT k = 0; k < mesh->mFaces[j].mNumIndices; ++k)
					{
						// Insert indices to model
						myMesh.indices.push_back(mesh->mFaces[j].mIndices[k]);
					}
				}

			}

			// Update total vertex and face count
			totalVertexCount += numVertices;
			totalFaceCount += numFaces;

			myMesh.MaterialIndex = mesh->mMaterialIndex;
			myMesh.FaceCount = numFaces;
			myMesh.VertexCount = numVertices;
			myMesh.Name = materials[myMesh.MaterialIndex].name;

			//------------------------------------------------------------
			// Read bones
			//------------------------------------------------------------
			if (mesh->HasBones())
			{
				for (UINT i = 0; i < mesh->mNumBones; ++i)				
				{
					aiBone* aibone = mesh->mBones[i];

					SkinnedDataStructs::Bone bone;
					bone.name = aibone->mName.C_Str();
					bone.offsetMatrix = XMFLOAT4X4(
						aibone->mOffsetMatrix.a1, aibone->mOffsetMatrix.a2, aibone->mOffsetMatrix.a3, aibone->mOffsetMatrix.a4,
						aibone->mOffsetMatrix.b1, aibone->mOffsetMatrix.b2, aibone->mOffsetMatrix.b3, aibone->mOffsetMatrix.b4,
						aibone->mOffsetMatrix.c1, aibone->mOffsetMatrix.c2, aibone->mOffsetMatrix.c3, aibone->mOffsetMatrix.c4,
						aibone->mOffsetMatrix.d1, aibone->mOffsetMatrix.d2, aibone->mOffsetMatrix.d3, aibone->mOffsetMatrix.d4
						);

					for (UINT j = 0; j < aibone->mNumWeights; ++j)
					{
						SkinnedDataStructs::VertexWeight vertexWeight;
						vertexWeight.VertexID = aibone->mWeights[j].mVertexId;
						vertexWeight.Weight = aibone->mWeights[j].mWeight;

						bone.weights.push_back(vertexWeight);
					}

					bone.meshIndex = curMesh;

					//myMesh.bones.push_back(bone);
					bones.push_back(bone);

					//globalBoneList.push_back(&myMesh.bones.back());
				}
			}

			// Set bone indices and all their weights for each vertex
// 			for (UINT i = 0; i < myMesh.vertices.size(); ++i)
// 			{
// 				for (UINT j = 0; myMesh.bones.size(); ++j)
// 				{
// 					for (UINT k = 0; k < myMesh.bones[j].weights.size(); ++k)
// 					{
// 						myMesh.vertices[myMesh.bones[j].weights[k].VertexID].boneIndices[k] = j;
// 
// 						switch (k)
// 						{
// 						case 0:
// 							myMesh.vertices[myMesh.bones[j].weights[k].VertexID].weights.x = myMesh.bones[j].weights[k].Weight;
// 							break;
// 						case 1:
// 							myMesh.vertices[myMesh.bones[j].weights[k].VertexID].weights.y = myMesh.bones[j].weights[k].Weight;
// 							break;
// 						case 2:
// 							myMesh.vertices[myMesh.bones[j].weights[k].VertexID].weights.z = myMesh.bones[j].weights[k].Weight;
// 							break;
// 						}
// 					}
// 				}
// 			}

			meshes.push_back(myMesh);

		} // Mesh end

		std::vector<XMFLOAT4X4> boneOffsets;

		for (UINT j = 0; j < bones.size(); ++j)
		{
			for (UINT k = 0; k < bones[j].weights.size(); ++k)
			{

				UINT VertexID = bones[j].weights[k].VertexID;
				// Find the mesh that contains this vertex

				// 					char buffer[20];
				// 					sprintf(buffer, "j:%d k:%d\n", j, k);
				// 					OutputDebugStringA(buffer);

				// Find which weight and respective bone index to fill
				if (VertexList[VertexID]->weights.x == 0.0f)
				{
					VertexList[VertexID]->weights.x = bones[j].weights[k].Weight;
					VertexList[VertexID]->boneIndices[0] = j;
				}
				else if (VertexList[VertexID]->weights.y == 0.0f)
				{
					VertexList[VertexID]->weights.y = bones[j].weights[k].Weight;
					VertexList[VertexID]->boneIndices[1] = j;
				}
				else if (VertexList[VertexID]->weights.z == 0.0f)
				{
					VertexList[VertexID]->weights.z = bones[j].weights[k].Weight;
					VertexList[VertexID]->boneIndices[2] = j;
				}


				//myMesh.vertices.at(myMesh.bones[j].weights[k].VertexID).boneIndices[k] = j;

				// 					switch (k)
				// 					{
				// 					case 0:
				// 						myMesh.vertices[myMesh.bones[j].weights[k].VertexID].weights.x = myMesh.bones[j].weights[k].Weight;
				// 						break;
				// 					case 1:
				// 						myMesh.vertices[myMesh.bones[j].weights[k].VertexID].weights.y = myMesh.bones[j].weights[k].Weight;
				// 						break;
				// 					case 2:
				// 						myMesh.vertices[myMesh.bones[j].weights[k].VertexID].weights.z = myMesh.bones[j].weights[k].Weight;
				// 						break;
				// 					}
			}
		}

		//------------------------------------------------------------
		// Read bones
		//------------------------------------------------------------
		// Iterate through all the bones in the scene
// 		for (UINT boneIndex = 0; boneIndex < allBones.size(); ++boneIndex)
// 		{
// 			// Get bone properties
// 			aiString boneName = allBones[boneIndex]->mName;
// 			aiMatrix4x4 offsetMatrix = allBones[boneIndex]->mOffsetMatrix;
// 			UINT numWeights = allBones[boneIndex]->mNumWeights;
// 
// 			//----------------------------------------------------------
// 			// Iterate through all vertices affected by this bone
// 			//----------------------------------------------------------
// 			for (UINT j = 0; j < numWeights; ++j)
// 			{
// 				// Get the index of the vertex
// 				aiVertexWeight weight = allBones[boneIndex]->mWeights[j];
// 				UINT vertexIndex = weight.mVertexId;
// 
// 				// Number of bones a vertex can be affected by (in most cases 4 is enough)
// 				UINT numPotentialBones = sizeof(allVertices[vertexIndex]->boneIndices);
// 
// 				// Find an empty slot (0 value) in the boneIndices array
// 				// (Array tells us the indices of the bones it's affected by)
// 				for (UINT k = 0; k < numPotentialBones; ++k)
// 				{
// 					if (allVertices[vertexIndex]->boneIndices[k] == 0)
// 					{
// 						// Set bone index
// 						allVertices[vertexIndex]->boneIndices[k] = boneIndex;
// 
// 						// Also set the weight this bone has
// 						switch(k)
// 						{
// 						case 0:
// 							allVertices[vertexIndex]->weights.x = weight.mWeight;
// 							break;
// 						case 1:
// 							allVertices[vertexIndex]->weights.y = weight.mWeight;
// 							break;
// 						case 2:
// 							allVertices[vertexIndex]->weights.z = weight.mWeight;
// 							break;
// 						default:
// 							break;
// 						}
// 
// 						break;
// 					}
// 				}
// 			} // Vertex end
// 
// 			//---------------------------------------------------------
// 			// Bone offsets
// 			//---------------------------------------------------------
// 			XMFLOAT4X4 boneOffset(	
// 				offsetMatrix.a1, offsetMatrix.a2, offsetMatrix.a3, offsetMatrix.a4,
// 				offsetMatrix.b1, offsetMatrix.b2, offsetMatrix.b3, offsetMatrix.b4,
// 				offsetMatrix.c1, offsetMatrix.c2, offsetMatrix.c3, offsetMatrix.c4,
// 				offsetMatrix.d1, offsetMatrix.d2, offsetMatrix.d3, offsetMatrix.d4);
// 
// 			boneOffsets.push_back(boneOffset);
// 
// 		} // Bone end

		//------------------------------------------------------------
		// Read animations
		//------------------------------------------------------------
		//std::vector<AnimationClip> animationClips;
		std::map<std::string, AnimationClip> myAnims;

		if (scene->HasAnimations())
		{
			// Iterate through all the animations in the scene
			for (UINT i = 0; i < scene->mNumAnimations; ++i)
			{
				aiAnimation* animation = scene->mAnimations[i];

				AnimationClip animationClip;

				// Iterate through each mesh channel for the animation
				for (UINT j = 0; j < animation->mNumChannels; ++j)
				{
					bool nodeIsBone = false;

					for (UINT k = 0; k < bones.size(); ++k)
					// Only add bone animation if node is a bone
					{
						if (animation->mChannels[j]->mNodeName.C_Str() == bones[k].name)
							nodeIsBone = true;
					}

					//animation->mChannels[j]->mNodeName.C_Str();

					if (!nodeIsBone)
						continue;

					BoneAnimation boneAnimation;

					aiNodeAnim* nodeAnim = animation->mChannels[j];

					readBoneKeyframes(nodeAnim, boneAnimation);

					//animationClip.boneAnimations.push_back(boneAnimation);
					myAnims[animation->mName.C_Str()].boneAnimations.push_back(boneAnimation);
				}

				// Push back the animation clip
				//animationClips.push_back(animationClip);
			}
		}

		//------------------------------------------------------------
		// Read bone hierarchy
		//------------------------------------------------------------
		// Traverse node tree (if node has a name it corresponds to a bone name)
		std::vector<int> BoneHierarchy;

		ReadNodeHierarchy(scene->mRootNode, bones, 0);

		for (UINT i = 0; i < meshes.size(); ++i)
		{
			for (UINT j = 0; j < bones.size(); ++j)
			{
				BoneHierarchy.push_back(bones[j].parentBoneIndex);
				boneOffsets.push_back(bones[j].offsetMatrix);
			}
		}
			

		// Set skin data
		skinData.set(BoneHierarchy, boneOffsets, myAnims);

	} // If scene contains mesh end

	// Clean up after reading file
	DefaultLogger::kill();

	return true;
}

//=================================================================================
// Loads all the materials from aiScene to our own material type
//=================================================================================
void GenericObjectLoader::loadMaterials(const aiScene* scene,
							std::vector<GenericMaterial>& materials)
{
	if (scene->HasMaterials())
	{
		for (UINT curMat = 0; curMat < scene->mNumMaterials; ++curMat)
		{
			// Get material name
			aiString name;
			scene->mMaterials[curMat]->Get(AI_MATKEY_NAME, name);

			// Get material properties
			aiColor3D ambient(0.0f, 0.0f, 0.0f);
			scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_AMBIENT, ambient);

			aiColor3D diffuse(0.0f, 0.0f, 0.0f);
			scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);

			aiColor3D specular(0.0f, 0.0f, 0.0f);
			scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_SPECULAR, specular);

			aiColor3D reflect(0.0f, 0.0f, 0.0f);
			scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_REFLECTIVE, reflect);

			// Create our own material
			GenericMaterial genMat;
			initMaterial(&genMat);

			// Set our material properties according to material we read
			genMat.name = name.C_Str();

			genMat.mat.Ambient.x = ambient.r;
			genMat.mat.Ambient.y = ambient.g;
			genMat.mat.Ambient.z = ambient.b;

			genMat.mat.Diffuse.x = diffuse.r;
			genMat.mat.Diffuse.y = diffuse.g;
			genMat.mat.Diffuse.z = diffuse.b;

			genMat.mat.Specular.x = specular.r;
			genMat.mat.Specular.y = specular.g;
			genMat.mat.Specular.z = specular.b;

			genMat.mat.Reflect.x = reflect.r;
			genMat.mat.Reflect.y = reflect.g;
			genMat.mat.Reflect.z = reflect.b;

			// Get diffuse texture
			aiString texPath;
			if (scene->mMaterials[curMat]->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == aiReturn_SUCCESS)
			{
				std::string texPathStr = texPath.C_Str();
				std::wstring tmp(texPathStr.begin(), texPathStr.end());


				// Remove characters like ".", "/" and "\" from the beginning
				// of the texture name
				wchar_t firstChar = tmp.front();
				while (firstChar == '.' || firstChar == '/' || firstChar == '\'')
				{
					tmp.erase(tmp.begin(), tmp.begin()+1);
					firstChar = tmp.front();
				}

				wchar_t curChar;
				int lastSlashIndex = -1;
				//int fileExtensionIndex = 0;
				// Remove all character before a slash and then the slash to remove
				// the absolute path (which will result in only the texture name)

				// But dont remove slashes after the actual texture name!
				for (UINT i = 0; i < tmp.size(); ++i)
				{
					curChar = tmp[i];

					// File extension found
					//if (curChar == '.')
						//fileExtensionIndex = i;

					// We dont want to remove slashes that are after 
					if (curChar == '\'' || curChar == '/')
						lastSlashIndex = i;
				}

				// At least one slash found
				if (lastSlashIndex != -1)
					tmp.erase(0, lastSlashIndex+1);

				// Replace "/" with "\" (when models have folders in their texture name) 
				for (UINT i = 0; i < tmp.size(); ++i)
				{
					if (tmp[i] == L'/')
					{
						tmp.replace(i, 1, L"\\");
					}
				}

				genMat.diffuseMapName = tmp;
			}

			// Get normal map
			aiString normalMapPath;
			if (scene->mMaterials[curMat]->GetTexture(aiTextureType_NORMALS, 0, &normalMapPath) == aiReturn_SUCCESS)
			{
				std::string normalMapPathStr = texPath.C_Str();
				std::wstring tmp(normalMapPathStr.begin(), normalMapPathStr.end());


				// Remove characters like ".", "/" and "\" from the beginning
				// of the texture name
				wchar_t firstChar = tmp.front();
				while (firstChar == '.' || firstChar == '/' || firstChar == '\'')
				{
					tmp.erase(tmp.begin(), tmp.begin()+1);
					firstChar = tmp.front();
				}

				// Replace "/" with "\" (when models have folders in their texture name) 
				for (UINT i = 0; i < tmp.size(); ++i)
				{
					if (tmp[i] == L'/')
					{
						tmp.replace(i, 1, L"\\");
					}
				}

				genMat.normalMapName = tmp;
			}

			// Push our material into the vector
			materials.push_back(genMat);
		}

	} // Materials end
}

//==================================================================================
// Material initializer (sets properties to default values)
//==================================================================================
void GenericObjectLoader::initMaterial(GenericMaterial* material)
{
	ZeroMemory(material, sizeof(material));

	material->mat.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	material->mat.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material->mat.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 16.0f);
	material->mat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	material->diffuseMapName = L"";
	material->normalMapName = L"";
	material->name = "";
}

//=================================================================================
// Reads keyframes from bone animations
//=================================================================================
void GenericObjectLoader::readBoneKeyframes(aiNodeAnim* nodeAnim,
							BoneAnimation& boneAnimation)
{
	UINT numPosKeys = nodeAnim->mNumPositionKeys;
	UINT numScaleKeys = nodeAnim->mNumScalingKeys;
	UINT numRotKeys = nodeAnim->mNumRotationKeys;

	// Find which type of key has the most values
	UINT* largestKeyType;
	largestKeyType = MathHelper::getMax(&numPosKeys, &numScaleKeys);
	largestKeyType = MathHelper::getMax(largestKeyType, &numRotKeys);

	boneAnimation.keyFrames.resize(*largestKeyType);

	// And set our keyframe vector to that size
// 	std::vector<KeyFrame> keyFrames;
// 	keyFrames.resize(*largestKeyType);

	for (UINT i = 0; i < boneAnimation.keyFrames.size(); ++i)
	{
		// First, initialize all the keyframes to default values
		boneAnimation.keyFrames[i].timePos = 0.0f;
		boneAnimation.keyFrames[i].translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		boneAnimation.keyFrames[i].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		boneAnimation.keyFrames[i].rotationQuat = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

		// Fill the time with the time from the key type with the most values
		if (*largestKeyType == numPosKeys)
			boneAnimation.keyFrames[i].timePos = (float)nodeAnim->mPositionKeys[i].mTime;
		else if (*largestKeyType == numScaleKeys)
			boneAnimation.keyFrames[i].timePos = (float)nodeAnim->mScalingKeys[i].mTime;
		else if (*largestKeyType == numRotKeys)
			boneAnimation.keyFrames[i].timePos = (float)nodeAnim->mRotationKeys[i].mTime;

		// Position values
		for (UINT j = 0; j < numPosKeys; ++j)
		{
			// Find the corresponding time frame
			if (boneAnimation.keyFrames[i].timePos == (float)nodeAnim->mPositionKeys[j].mTime)
			{
				// Then read all the position keys
				boneAnimation.keyFrames[i].translation.x = nodeAnim->mPositionKeys[i].mValue.x;
				boneAnimation.keyFrames[i].translation.y = nodeAnim->mPositionKeys[i].mValue.y;
				boneAnimation.keyFrames[i].translation.z = nodeAnim->mPositionKeys[i].mValue.z;
				break;
			}
		}

		// Scaling values
		for (UINT j = 0; j < numScaleKeys; ++j)
		{
			// Find the corresponding time frame
			if (boneAnimation.keyFrames[i].timePos == (float)nodeAnim->mScalingKeys[j].mTime)
			{
				boneAnimation.keyFrames[i].scale.x = nodeAnim->mScalingKeys[i].mValue.x;
				boneAnimation.keyFrames[i].scale.x = nodeAnim->mScalingKeys[i].mValue.y;
				boneAnimation.keyFrames[i].scale.x = nodeAnim->mScalingKeys[i].mValue.z;
				break;
			}
		}

		// Rotation values
		for (UINT j = 0; j < numRotKeys; ++j)
		{
			// Find the corresponding time frame
			if (boneAnimation.keyFrames[i].timePos == (float)nodeAnim->mRotationKeys[j].mTime)
			{
			boneAnimation.keyFrames[i].rotationQuat.x = nodeAnim->mRotationKeys[j].mValue.x;
			boneAnimation.keyFrames[i].rotationQuat.y = nodeAnim->mRotationKeys[j].mValue.y;
			boneAnimation.keyFrames[i].rotationQuat.z = nodeAnim->mRotationKeys[j].mValue.z;
			boneAnimation.keyFrames[i].rotationQuat.w = nodeAnim->mRotationKeys[j].mValue.w;
			break;
			}
		}
	}
}

void GenericObjectLoader::ReadNodeHierarchy(const aiNode* node, std::vector<SkinnedDataStructs::Bone>& bones, int parentBoneIndex)
{
	std::string nodeName = node->mName.C_Str();

	bool nodeIsBone = false;
	int boneIndex = 0;

	// Find bone in bone vector
	for (UINT j = 0; j < bones.size(); ++j)
	{
		if (bones[j].name == nodeName)
		{
			boneIndex = j;
			nodeIsBone = true;
		}
	}



	if (nodeIsBone)
		bones[boneIndex].parentBoneIndex = parentBoneIndex;

	for (UINT i = 0; i < node->mNumChildren; ++i)
		ReadNodeHierarchy(node->mChildren[i], bones, boneIndex);
}

//===================================================================================
// Object loader
//===================================================================================
bool GenericObjectLoader::loadObject(const std::string& fileName,
	std::vector<Vertex::Basic32>& vertices,
	std::vector<UINT>& indices,
	std::vector<MeshGeometry::Subset>& subsets,
	std::vector<GenericMaterial>& materials)
{
	using namespace Assimp;

	// Create instance of assimp Importer class
	Importer importer;

	std::string logName = "AssimpLog.log";

	DefaultLogger::create(logName.c_str(), Logger::VERBOSE);

	bool isTriangulated = true;

	// Read file with post processing flags
	const aiScene* scene = importer.ReadFile(fileName,
		aiProcess_CalcTangentSpace			|	// Generate tangents and bitangents
		aiProcess_Triangulate				|	// Triangulate
		// 		aiProcess_JoinIdenticalVertices		|	// Avoid duplicate vertices
		aiProcess_GenSmoothNormals			|	// Generate vertex normals if model doesn't contain them
		aiProcess_GenUVCoords				|	// Generate texture coordinates if model doesn't contain them
		// 		//aiProcess_ValidateDataStructure		|	// Validate imported scene data structure (valid indices etc.)
		aiProcess_RemoveRedundantMaterials	|	// Remove materials never referenced
		// 		//aiProcess_OptimizeMeshes			|	// Reduce number of meshes
		// 		//aiProcess_OptimizeGraph				|	// Optimize scene hierarchy
		// 		aiProcess_SortByPType				|	// Split meshes that contains multiple primitive types
		//aiProcess_ImproveCacheLocality |
		aiProcess_LimitBoneWeights |
		aiProcess_SplitLargeMeshes |
		aiProcess_SortByPType |
		//aiProcess_FindDegenerates |
		//aiProcess_FindInvalidData |
		aiProcess_ConvertToLeftHanded 			// Make compatible with Direct3D
		//aiProcessPreset_TargetRealtime_Quality // Combination of post processing flags
		//aiProcess_FindInvalidData				// Find invalid data
		);										

	// Failed reading file
	if (!scene)
	{
		// Error logging
		DefaultLogger::get()->info("Failed reading file!");
		DefaultLogger::kill();

		return false;
	}

	//----------------------------------------------------
	// Read the scene
	//----------------------------------------------------

	// Check whether or not it has read at least one or more meshes
	// Assimp splits up a mesh if more than one material was found
	// (So it splits up an object into subsets)
	if (scene->HasMeshes())
	{
		UINT totalVertexCount = 0;
		UINT totalFaceCount = 0;

		// Get scene root node
		aiNode* rootNode;
		rootNode = scene->mRootNode;

		aiNode* curNode = rootNode;

		// Iterate through node children
		for (UINT i = 0; i < curNode->mNumChildren; ++i)
		{
			std::stringstream sstream;
			sstream << "Node: " << curNode->mChildren[i]->mName.C_Str();

			// Get childrens mesh indexes
			for (UINT j = 0; j < curNode->mChildren[i]->mNumMeshes; ++j)
			{
				sstream << " (Mesh index: " << curNode->mChildren[i]->mMeshes[j] << ")";
				sstream << " (Material index: " << scene->mMeshes[curNode->mChildren[i]->mMeshes[j]]->mMaterialIndex << ")";
			}

			sstream << " (Num children: " << curNode->mChildren[i]->mNumChildren << ")";

			DefaultLogger::get()->info(sstream.str().c_str());
		}

		//------------------------------------------------------------
		// Read materials
		//------------------------------------------------------------
		if (scene->HasMaterials())
		{
			for (UINT curMat = 0; curMat < scene->mNumMaterials; ++curMat)
			{
				// Get material name
				aiString name;
				scene->mMaterials[curMat]->Get(AI_MATKEY_NAME, name);

				// Get material properties
				aiColor3D ambient(0.0f, 0.0f, 0.0f);
				scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_AMBIENT, ambient);

				aiColor3D diffuse(0.0f, 0.0f, 0.0f);
				scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);

				aiColor3D specular(0.0f, 0.0f, 0.0f);
				scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_SPECULAR, specular);

				aiColor3D reflect(0.0f, 0.0f, 0.0f);
				scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_REFLECTIVE, reflect);

				// Create our own material
				GenericMaterial genMat;
				initMaterial(&genMat);

				// Set our material properties according to material we read
				genMat.name = name.C_Str();

				genMat.mat.Ambient.x = ambient.r;
				genMat.mat.Ambient.y = ambient.g;
				genMat.mat.Ambient.z = ambient.b;

				genMat.mat.Diffuse.x = diffuse.r;
				genMat.mat.Diffuse.y = diffuse.g;
				genMat.mat.Diffuse.z = diffuse.b;

				genMat.mat.Specular.x = specular.r;
				genMat.mat.Specular.y = specular.g;
				genMat.mat.Specular.z = specular.b;

				genMat.mat.Reflect.x = reflect.r;
				genMat.mat.Reflect.y = reflect.g;
				genMat.mat.Reflect.z = reflect.b;

				// Get texture name
				aiString texPath;
				if (AI_SUCCESS == scene->mMaterials[curMat]->GetTexture(aiTextureType_DIFFUSE, 0, &texPath))
				{
					std::string texPathStr = texPath.C_Str();
					std::wstring tmp(texPathStr.begin(), texPathStr.end());

					char chars[] = "'/''\'";

					// Remove any slashes from texture name
					for (UINT i = 0; i < strlen(chars); ++i)
						tmp.erase(std::remove(tmp.begin(), tmp.end(), chars[i]), tmp.end());

					genMat.diffuseMapName = tmp;
				}

				// Push our material into the vector
				materials.push_back(genMat);
			}

		} // Materials end

		//----------------------------------------------------------
		// Read all scene meshes
		//----------------------------------------------------------
		for (UINT curMesh = 0; curMesh < scene->mNumMeshes; ++curMesh)
		{
			// Get mesh name
			aiMesh* mesh = scene->mMeshes[curMesh];
			aiString meshName = mesh->mName;

			// Number of vertices and faces
			UINT numVertices = mesh->mNumVertices;
			UINT numFaces = mesh->mNumFaces;

			//------------------------------------------------------------
			// Create subset
			//------------------------------------------------------------
			MeshGeometry::Subset subset;
			ZeroMemory(&subset, sizeof(subset));

			// Get material index used by this subset
			subset.MaterialIndex = mesh->mMaterialIndex;

			subset.Id = curMesh;
			subset.Name = materials[subset.MaterialIndex].name;
			subset.VertexCount = numVertices;
			subset.FaceCount = numFaces;

			// Subset offset
			subset.FaceStart = totalFaceCount;
			subset.VertexStart = totalVertexCount;

			//------------------------------------------------------------
			// Read vertices
			//------------------------------------------------------------
			for (UINT j = 0; j < numVertices; ++j)
			{
				Vertex::Basic32 vertex;
				ZeroMemory(&vertex, sizeof(vertex));

				// Vertex position
				vertex.position.x = mesh->mVertices[j].x;
				vertex.position.y = mesh->mVertices[j].y;
				vertex.position.z = mesh->mVertices[j].z;

				// Vertex normal
				vertex.normal.x = mesh->mNormals[j].x;
				vertex.normal.y = mesh->mNormals[j].y;
				vertex.normal.z = mesh->mNormals[j].z;

				// Vertex texture coordinates (UV)
				vertex.texCoord.x = mesh->mTextureCoords[0][j].x;
				vertex.texCoord.y = mesh->mTextureCoords[0][j].y;

				// Insert vertex to model
				vertices.push_back(vertex);
			}

			//------------------------------------------------------------
			// Read indices
			//------------------------------------------------------------
			// Reminder: AI_SCENE_FLAGS_NON_VERBOSE_FORMAT is set by the
			// aiProcess_JoinIdenticalVertices post processing flag, which
			// means that each face reference a unique set of vertices.
			// Here you could check if AI_SCENE_FLAGS_NON_VERBOSE_FORMAT is
			// true or not, and handle it accordingly.

			for (UINT j = 0; j < numFaces; ++j)
			{
				// If mesh is triangulated
				if (isTriangulated)
				{
					// Always three indices per face
					for (UINT k = 0; k < 3; ++k)
					{
						// Insert indices to model
						indices.push_back(mesh->mFaces[j].mIndices[k]);
					}
				}

				// Else find out how many indices there are per face
				else
				{
					for (UINT k = 0; k < mesh->mFaces[j].mNumIndices; ++k)
					{
						// Insert indices to model
						indices.push_back(mesh->mFaces[j].mIndices[k]);
					}
				}

			}

			// Update total vertex and face count
			totalVertexCount += numVertices;
			totalFaceCount += numFaces;

			// Insert new subset to subset vector
			subsets.push_back(subset);

		} // Mesh end

	} // If contains mesh end


	// Clean up after reading file
	DefaultLogger::kill();

	return true;
}

//---------------------

//===================================================================================
// Skinned object loader (objects with bones)
//===================================================================================
bool GenericObjectLoader::loadSkinnedObject(const std::string& fileName,
	std::vector<Vertex::Basic32>& vertices,
	std::vector<UINT>& indices,
	std::vector<MeshGeometry::Subset>& subsets,
	std::vector<GenericMaterial>& materials,
	SkinnedData& skinData)
{
	using namespace Assimp;

	// Create instance of assimp Importer class
	Importer importer;

	DefaultLogger::create("TestLog.txt", Logger::VERBOSE);

	bool isTriangulated = true;

	// Read file with post processing flags
	const aiScene* scene = importer.ReadFile(fileName,
		// 		aiProcess_CalcTangentSpace			|	// Generate tangents and bitangents
		// 		aiProcess_Triangulate				|	// Triangulate
		// 		aiProcess_JoinIdenticalVertices		|	// Avoid duplicate vertices
		// 		aiProcess_GenSmoothNormals			|	// Generate vertex normals if model doesn't contain them
		// 		aiProcess_GenUVCoords				|	// Generate texture coordinates if model doesn't contain them
		// 		//aiProcess_ValidateDataStructure		|	// Validate imported scene data structure (valid indices etc.)
		// 		aiProcess_RemoveRedundantMaterials	|	// Remove materials never referenced
		// 		//aiProcess_OptimizeMeshes			|	// Reduce number of meshes
		// 		//aiProcess_OptimizeGraph				|	// Optimize scene hierarchy
		// 		aiProcess_SortByPType				|	// Split meshes that contains multiple primitive types
		// 		aiProcess_ImproveCacheLocality |
		// 		aiProcess_LimitBoneWeights |
		// 		aiProcess_SplitLargeMeshes |
		// 		aiProcess_SortByPType |
		// 		//aiProcess_FindDegenerates |
		// 		aiProcess_FindInvalidData |
		aiProcess_ConvertToLeftHanded |			// Make compatible with Direct3D
		aiProcessPreset_TargetRealtime_Quality // Combination of post processing flags
		//aiProcess_FindInvalidData				// Find invalid data
		);										

	// Failed reading file
	if (!scene)
	{
		// Error logging
		DefaultLogger::get()->info("Failed reading file!");
		DefaultLogger::kill();

		return false;
	}

	//----------------------------------------------------
	// Read the scene
	//----------------------------------------------------

	// Check whether or not it has read at least one or more meshes
	// Assimp splits up a mesh if more than one material was found
	// (So it splits up an object into subsets)
	if (scene->HasMeshes())
	{
		UINT totalVertexCount = 0;
		UINT totalFaceCount = 0;

		//aiNode* sceneRoot = scene->mRootNode;

		//----------------------------------------------------------
		// Mesh iteration
		//----------------------------------------------------------
		for (UINT curMesh = 0; curMesh < scene->mNumMeshes; ++curMesh)
		{
			aiMesh* mesh = scene->mMeshes[curMesh];

			// Create subset
			MeshGeometry::Subset subset;
			subset.VertexCount = 0;
			subset.FaceCount = 0;

			// Get mesh name
			aiString meshName = mesh->mName;
			subset.Name = meshName.C_Str();

			subset.Id = curMesh;
			subset.FaceStart = totalFaceCount;
			subset.VertexStart = totalVertexCount;

			// Find material used by this subset
			subset.MaterialIndex = mesh->mMaterialIndex;

			//------------------------------------------------------------
			// Read vertices
			//------------------------------------------------------------
			UINT vertexCount = mesh->mNumVertices;
			subset.VertexCount = vertexCount;

			for (UINT j = 0; j < vertexCount; ++j)
			{
				Vertex::Basic32 vertex;

				// Vertex position
				vertex.position.x = mesh->mVertices[j].x;
				vertex.position.y = mesh->mVertices[j].y;
				vertex.position.z = mesh->mVertices[j].z;

				// Vertex normal
				vertex.normal.x = mesh->mNormals[j].x;
				vertex.normal.y = mesh->mNormals[j].y;
				vertex.normal.z = mesh->mNormals[j].z;

				// Vertex texture coordinates (UV)
				vertex.texCoord.x = mesh->mTextureCoords[0][j].x;
				vertex.texCoord.y = mesh->mTextureCoords[0][j].y;

				// Insert vertex to model
				vertices.push_back(vertex);
			}

			//------------------------------------------------------------
			// Read faces
			//------------------------------------------------------------
			UINT faceCount = mesh->mNumFaces;
			subset.FaceCount = faceCount;

			for (UINT j = 0; j < faceCount; ++j)
			{
				// If mesh is triangulated
				if (isTriangulated)
				{
					// Always three indices per face
					for (UINT k = 0; k < 3; ++k)
					{
						// Insert indices to model
						indices.push_back(mesh->mFaces[j].mIndices[k]);
					}
				}

				// Else find out how many indices there are per face
				else
				{
					for (UINT k = 0; k < mesh->mFaces[j].mNumIndices; ++k)
					{
						// Insert indices to model
						indices.push_back(mesh->mFaces[j].mIndices[k]);
					}
				}

			}

			//------------------------------------------------------------
			// Read bones
			//------------------------------------------------------------

			for (UINT j = 0; j < mesh->mNumBones; ++j)
			{
			}

			//-------------------------------------------------------------
			// Mesh end
			//-------------------------------------------------------------
			// Update total vertex and face count
			totalVertexCount += vertexCount;
			totalFaceCount += faceCount;

			// Insert new subset to subset vector
			subsets.push_back(subset);
		}

		//------------------------------------------------------------
		// Read materials
		//------------------------------------------------------------
		if (scene->HasMaterials())
		{
			for (UINT curMat = 0; curMat < scene->mNumMaterials; ++curMat)
			{
				// Get material name
				aiString name;
				scene->mMaterials[curMat]->Get(AI_MATKEY_NAME, name);
				std::string nameStr = name.C_Str();

				GenericMaterial genMat;
				initMaterial(&genMat);

				genMat.name = nameStr;

				// Get material properties and insert them to our material
				aiColor3D ambient(0.0f, 0.0f, 0.0f);
				scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_AMBIENT, ambient);

				aiColor3D diffuse(0.0f, 0.0f, 0.0f);
				scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);

				aiColor3D specular(0.0f, 0.0f, 0.0f);
				scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_SPECULAR, specular);

				genMat.mat.Ambient.x = ambient.r;
				genMat.mat.Ambient.y = ambient.g;
				genMat.mat.Ambient.z = ambient.b;

				genMat.mat.Diffuse.x = diffuse.r;
				genMat.mat.Diffuse.y = diffuse.g;
				genMat.mat.Diffuse.z = diffuse.b;

				genMat.mat.Specular.x = specular.r;
				genMat.mat.Specular.y = specular.g;
				genMat.mat.Specular.z = specular.b;

				// Get texture
				aiString texPath;
				if (AI_SUCCESS == scene->mMaterials[curMat]->GetTexture(aiTextureType_DIFFUSE, 0, &texPath))
				{
					std::string texPathStr = texPath.C_Str();
					std::wstring tmp(texPathStr.begin(), texPathStr.end());
					genMat.diffuseMapName = tmp;
				}

				materials.push_back(genMat);
			}
		}
	}

	DefaultLogger::kill();

	return true;
}

//==================================================================================
// Object loader with meshes (OLD)
//==================================================================================
/*
bool GenericObjectLoader::loadObject(const std::string& fileName,
	std::vector<GenericMaterial>& materials,
	std::vector<GenericMesh>& meshes,
	ID3D11Device* device)
{
	using namespace Assimp;

	// Create instance of assimp Importer class
	Importer importer;

	std::string logName = "AssimpMeshLoaderLog.log";

	DefaultLogger::create(logName.c_str(), Logger::VERBOSE);

	bool isTriangulated = true;

	// Read file with post processing flags
	const aiScene* scene = importer.ReadFile(fileName,
		aiProcess_CalcTangentSpace			|	// Generate tangents and bitangents
		aiProcess_Triangulate				|	// Triangulate
		// 		aiProcess_JoinIdenticalVertices		|	// Avoid duplicate vertices
		aiProcess_GenSmoothNormals			|	// Generate vertex normals if model doesn't contain them
		aiProcess_GenUVCoords				|	// Generate texture coordinates if model doesn't contain them
		// 		//aiProcess_ValidateDataStructure		|	// Validate imported scene data structure (valid indices etc.)
		aiProcess_RemoveRedundantMaterials	|	// Remove materials never referenced
		// 		//aiProcess_OptimizeMeshes			|	// Reduce number of meshes
		// 		//aiProcess_OptimizeGraph				|	// Optimize scene hierarchy
		// 		aiProcess_SortByPType				|	// Split meshes that contains multiple primitive types
		//aiProcess_ImproveCacheLocality |
		aiProcess_LimitBoneWeights |
		aiProcess_SplitLargeMeshes |
		aiProcess_SortByPType |
		//aiProcess_FindDegenerates |
		//aiProcess_FindInvalidData |
		aiProcess_ConvertToLeftHanded 			// Make compatible with Direct3D
		//aiProcessPreset_TargetRealtime_Quality // Combination of post processing flags
		//aiProcess_FindInvalidData				// Find invalid data
		);										

	// Failed reading file
	if (!scene)
	{
		// Error logging
		DefaultLogger::get()->info("Failed reading file!");
		DefaultLogger::kill();

		return false;
	}

	//----------------------------------------------------
	// Read the scene
	//----------------------------------------------------

	// Check whether or not it has read at least one or more meshes
	// Assimp splits up a mesh if more than one material was found
	// (So it splits up an object into subsets)
	if (scene->HasMeshes())
	{
		UINT totalVertexCount = 0;
		UINT totalFaceCount = 0;

		// Get scene root node
		aiNode* rootNode;
		rootNode = scene->mRootNode;

		aiNode* curNode = rootNode;

		// Iterate through node children
		for (UINT i = 0; i < curNode->mNumChildren; ++i)
		{
			std::stringstream sstream;
			sstream << "Node: " << curNode->mChildren[i]->mName.C_Str();

			// Get childrens mesh indexes
			for (UINT j = 0; j < curNode->mChildren[i]->mNumMeshes; ++j)
			{
				sstream << " (Mesh index: " << curNode->mChildren[i]->mMeshes[j] << ")";
				sstream << " (Material index: " << scene->mMeshes[curNode->mChildren[i]->mMeshes[j]]->mMaterialIndex << ")";
			}

			sstream << " (Num children: " << curNode->mChildren[i]->mNumChildren << ")";

			DefaultLogger::get()->info(sstream.str().c_str());
		}

		//------------------------------------------------------------
		// Read materials
		//------------------------------------------------------------
		if (scene->HasMaterials())
		{
			for (UINT curMat = 0; curMat < scene->mNumMaterials; ++curMat)
			{
				// Get material name
				aiString name;
				scene->mMaterials[curMat]->Get(AI_MATKEY_NAME, name);

				// Get material properties
				aiColor3D ambient(0.0f, 0.0f, 0.0f);
				scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_AMBIENT, ambient);

				aiColor3D diffuse(0.0f, 0.0f, 0.0f);
				scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);

				aiColor3D specular(0.0f, 0.0f, 0.0f);
				scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_SPECULAR, specular);

				aiColor3D reflect(0.0f, 0.0f, 0.0f);
				scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_REFLECTIVE, reflect);

				// Create our own material
				GenericMaterial genMat;
				initMaterial(&genMat);

				// Set our material properties according to material we read
				genMat.name = name.C_Str();

				genMat.mat.Ambient.x = ambient.r;
				genMat.mat.Ambient.y = ambient.g;
				genMat.mat.Ambient.z = ambient.b;

				genMat.mat.Diffuse.x = diffuse.r;
				genMat.mat.Diffuse.y = diffuse.g;
				genMat.mat.Diffuse.z = diffuse.b;

				genMat.mat.Specular.x = specular.r;
				genMat.mat.Specular.y = specular.g;
				genMat.mat.Specular.z = specular.b;

				genMat.mat.Reflect.x = reflect.r;
				genMat.mat.Reflect.y = reflect.g;
				genMat.mat.Reflect.z = reflect.b;

				// Get texture name
				aiString texPath;
				if (AI_SUCCESS == scene->mMaterials[curMat]->GetTexture(aiTextureType_DIFFUSE, 0, &texPath))
				{
					std::string texPathStr = texPath.C_Str();
					std::wstring tmp(texPathStr.begin(), texPathStr.end());

					char chars[] = "'/''\'";

					// Remove any slashes from texture name
					for (UINT i = 0; i < strlen(chars); ++i)
						tmp.erase(std::remove(tmp.begin(), tmp.end(), chars[i]), tmp.end());

					genMat.diffuseMapName = tmp;
				}

				// Push our material into the vector
				materials.push_back(genMat);
			}

		} // Materials end

		//----------------------------------------------------------
		// Read all scene meshes
		//----------------------------------------------------------
		for (UINT curMesh = 0; curMesh < scene->mNumMeshes; ++curMesh)
		{
			// Get mesh name
			aiMesh* mesh = scene->mMeshes[curMesh];
			aiString meshName = mesh->mName;

			// Number of vertices and faces
			UINT numVertices = mesh->mNumVertices;
			UINT numFaces = mesh->mNumFaces;

			//------------------------------------------------------------
			// Create mesh
			//------------------------------------------------------------
			GenericMesh myMesh;

			//------------------------------------------------------------
			// Read vertices
			//------------------------------------------------------------
			for (UINT j = 0; j < numVertices; ++j)
			{
				Vertex::Basic32 vertex;
				ZeroMemory(&vertex, sizeof(vertex));

				// Vertex position
				vertex.position.x = mesh->mVertices[j].x;
				vertex.position.y = mesh->mVertices[j].y;
				vertex.position.z = mesh->mVertices[j].z;

				// Vertex normal
				vertex.normal.x = mesh->mNormals[j].x;
				vertex.normal.y = mesh->mNormals[j].y;
				vertex.normal.z = mesh->mNormals[j].z;

				// Vertex texture coordinates (UV)
				vertex.texCoord.x = mesh->mTextureCoords[0][j].x;
				vertex.texCoord.y = mesh->mTextureCoords[0][j].y;

				// Insert vertex to model
				myMesh.vertices.push_back(vertex);
			}

			//------------------------------------------------------------
			// Read indices
			//------------------------------------------------------------
			// Reminder: AI_SCENE_FLAGS_NON_VERBOSE_FORMAT is set by the
			// aiProcess_JoinIdenticalVertices post processing flag, which
			// means that each face reference a unique set of vertices.
			// Here you could check if AI_SCENE_FLAGS_NON_VERBOSE_FORMAT is
			// true or not, and handle it accordingly.

			for (UINT j = 0; j < numFaces; ++j)
			{
				// If mesh is triangulated
				if (isTriangulated)
				{
					// Always three indices per face
					for (UINT k = 0; k < 3; ++k)
					{
						// Insert indices to model
						myMesh.indices.push_back(mesh->mFaces[j].mIndices[k]);
					}
				}

				// Else find out how many indices there are per face
				else
				{
					for (UINT k = 0; k < mesh->mFaces[j].mNumIndices; ++k)
					{
						// Insert indices to model
						myMesh.indices.push_back(mesh->mFaces[j].mIndices[k]);
					}
				}

			}

			// Update total vertex and face count
			totalVertexCount += numVertices;
			totalFaceCount += numFaces;

			myMesh.setIndices(device, &myMesh.indices[0], myMesh.indices.size());
			myMesh.setVertices(device, &myMesh.vertices[0], myMesh.vertices.size());

			myMesh.MaterialIndex = mesh->mMaterialIndex;
			myMesh.FaceCount = numFaces;
			myMesh.VertexCount = numVertices;
			myMesh.Name = materials[myMesh.MaterialIndex].name;

			meshes.push_back(myMesh);

		} // Mesh end

	} // If contains mesh end


	// Clean up after reading file
	DefaultLogger::kill();

	return true;
}
*/

/*
//=====================================================================================================
//
//=====================================================================================================
bool GenericObjectLoader::loadObject(const std::string& fileName, std::vector<GenericMaterial>& materials,
	Vertex::Basic32** vertices, UINT** indices, std::vector<GenericMesh>& meshes, ID3D11Device* device)
{
	using namespace Assimp;

	// Create instance of assimp Importer class
	Importer importer;

	DefaultLogger::create("TestLog.txt", Logger::VERBOSE);

	bool isTriangulated = true;

	// Read file with post processing flags
	const aiScene* scene = importer.ReadFile(fileName,
// 		aiProcess_CalcTangentSpace			|	// Generate tangents and bitangents
// 		aiProcess_Triangulate				|	// Triangulate
// 		//aiProcess_JoinIdenticalVertices		|	// Avoid duplicate vertices
// 		aiProcess_GenSmoothNormals			|	// Generate vertex normals if model doesn't contain them
// 		aiProcess_GenUVCoords				|	// Generate texture coordinates if model doesn't contain them
// 		//aiProcess_ValidateDataStructure		|	// Validate imported scene data structure (valid indices etc.)
// 		aiProcess_RemoveRedundantMaterials	|	// Remove materials never referenced
// 		//aiProcess_OptimizeMeshes			|	// Reduce number of meshes
// 		//aiProcess_OptimizeGraph				|	// Optimize scene hierarchy
// 		aiProcess_SortByPType				|	// Split meshes that contains multiple primitive types
// 		aiProcess_ImproveCacheLocality |
// 		aiProcess_LimitBoneWeights |
// 		aiProcess_SplitLargeMeshes |
// 		aiProcess_SortByPType |
// 		//aiProcess_FindDegenerates |
// 		aiProcess_FindInvalidData |
		aiProcess_ConvertToLeftHanded |			// Make compatible with Direct3D
		aiProcessPreset_TargetRealtime_Quality // Combination of post processing flags
		//aiProcess_FindInvalidData				// Find invalid data
		);										

	// Failed reading file
	if (!scene)
	{
		// Error logging
		DefaultLogger::get()->info("Failed reading file!");
		DefaultLogger::kill();

		return false;
	}

	//----------------------------------------------------
	// Read the scene
	//----------------------------------------------------

	// Check whether or not it has read at least one or more meshes
	// Assimp splits up a mesh if more than one material was found
	// (So it splits up an object into subsets)
	if (scene->HasMeshes())
	{
		UINT totalVertexCount = 0;
		UINT totalFaceCount = 0;

		//aiNode* sceneRoot = scene->mRootNode;

		//----------------------------------------------------------
		// Mesh iteration
		//----------------------------------------------------------
		for (UINT curMesh = 0; curMesh < scene->mNumMeshes; ++curMesh)
		{
			// Empty vertex and index vectors
			//vertices.erase(vertices.begin(), vertices.end());
			//indices.erase(indices.begin(), indices.end());

			// Get current mesh pointer
			aiMesh* mesh = scene->mMeshes[curMesh];

			// Mesh we later insert to our mesh vector
			GenericMesh meshCopy;

			meshCopy.MaterialIndex = mesh->mMaterialIndex;

			//------------------------------------------------------------
			// Read vertices
			//------------------------------------------------------------
			UINT vertexCount = mesh->mNumVertices;
			vertices = new Vertex::Basic32*[vertexCount];
			meshCopy.VertexCount = vertexCount;

			for (UINT j = 0; j < vertexCount; ++j)
			{
				Vertex::Basic32 vertex;

				// Vertex position
				vertex.position.x = mesh->mVertices[j].x;
				vertex.position.y = mesh->mVertices[j].y;
				vertex.position.z = mesh->mVertices[j].z;

				// Vertex normal
				vertex.normal.x = mesh->mNormals[j].x;
				vertex.normal.y = mesh->mNormals[j].y;
				vertex.normal.z = mesh->mNormals[j].z;

				// Vertex texture coordinates (UV)
				vertex.texCoord.x = mesh->mTextureCoords[0][j].x;
				vertex.texCoord.y = mesh->mTextureCoords[0][j].y;

				vertices[j] = new Vertex::Basic32(vertex);
			}

			//------------------------------------------------------------
			// Read faces
			//------------------------------------------------------------
			UINT faceCount = mesh->mNumFaces;

			indices = new UINT*[faceCount*3];

			UINT indexCount = 0;

			for (UINT j = 0; j < faceCount; ++j)
			{
				// If mesh is triangulated
				if (isTriangulated)
				{
					// Always three indices per face
					for (UINT k = 0; k < 3; ++k)
					{
						// Insert indices to model
						//indices.push_back(mesh->mFaces[j].mIndices[k]);
						indices[indexCount] = new UINT(mesh->mFaces[j].mIndices[k]);
						indexCount++;
					}
				}

				// Else find out how many indices there are per face
				else
				{
					for (UINT k = 0; k < mesh->mFaces[j].mNumIndices; ++k)
					{
						// Insert indices to model
						//indices.push_back(mesh->mFaces[j].mIndices[k]);
					}
				}

			}

			//-------------------------------------------------------------
			// Mesh end
			//-------------------------------------------------------------
			// Update total vertex and face count
			totalVertexCount += vertexCount;
			totalFaceCount += faceCount;

			meshCopy.FaceCount = mesh->mNumFaces;

			//meshCopy.setVertices(device, &vertices[0], vertices.size());
			//meshCopy.setIndices(device, &indices[0], indices.size());

			meshes.push_back(meshCopy);
		}

		//------------------------------------------------------------
		// Read materials
		//------------------------------------------------------------
		if (scene->HasMaterials())
		{
			for (UINT curMat = 0; curMat < scene->mNumMaterials; ++curMat)
			{
				// Get material name
				aiString name;
				scene->mMaterials[curMat]->Get(AI_MATKEY_NAME, name);
				std::string nameStr = name.C_Str();

				// Skip adding default material automatically created by assimp
				// (If there exists a user defined material)
				//if (nameStr != AI_DEFAULT_MATERIAL_NAME && scene->mNumMaterials > 1)
				//{				
				GenericMaterial genMat;
				initMaterial(&genMat);

				genMat.name = nameStr;

				// Get material properties and insert them to our material
				aiColor3D ambient(0.0f, 0.0f, 0.0f);
				scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_AMBIENT, ambient);

				aiColor3D diffuse(0.0f, 0.0f, 0.0f);
				scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);

				aiColor3D specular(0.0f, 0.0f, 0.0f);
				scene->mMaterials[curMat]->Get(AI_MATKEY_COLOR_SPECULAR, specular);

				genMat.mat.Ambient.x = ambient.r;
				genMat.mat.Ambient.y = ambient.g;
				genMat.mat.Ambient.z = ambient.b;

				genMat.mat.Diffuse.x = diffuse.r;
				genMat.mat.Diffuse.y = diffuse.g;
				genMat.mat.Diffuse.z = diffuse.b;

				genMat.mat.Specular.x = specular.r;
				genMat.mat.Specular.y = specular.g;
				genMat.mat.Specular.z = specular.b;

				// Get texture
				aiString texPath;
				if (AI_SUCCESS == scene->mMaterials[curMat]->GetTexture(aiTextureType_DIFFUSE, 0, &texPath))
				{
					std::string texPathStr = texPath.C_Str();
					std::wstring tmp(texPathStr.begin(), texPathStr.end());
					genMat.diffuseMapName = tmp;
				}

				materials.push_back(genMat);
				//}
			}
		}
	}

	DefaultLogger::kill();

	return true;
}
*/