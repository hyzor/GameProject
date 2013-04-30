#include "GenericObjectLoader.h"

GenericObjectLoader::GenericObjectLoader(void)
{
}

GenericObjectLoader::~GenericObjectLoader(void)
{
}

//===================================================================================
// Skinned object loader (objects with bones)
//===================================================================================
bool GenericObjectLoader::loadSkinnedObject(const std::string& fileName,
	std::vector<GenericMaterial>& materials,
	std::vector<GenericSkinnedMesh>& meshes)
{
	using namespace Assimp;

	// Create instance of assimp Importer class
	Importer importer;

	std::string logName = "Logs\\AssimpMeshLoader.log";

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
		//aiNode* rootNode = scene->mRootNode;

		// Get bone root
		//SkinData::Bone* RootBone = CreateBoneTree(scene->mRootNode, NULL);

		std::vector<Vertex::PosNormalTexTanSkinned*> VertexList;

		//------------------------------------------------------------
		// Read materials
		//------------------------------------------------------------
		ReadMaterials(scene, materials);

		//----------------------------------------------------------
		// Read all scene meshes
		//----------------------------------------------------------
		for (UINT curMeshIndex = 0; curMeshIndex < scene->mNumMeshes; ++curMeshIndex)
		{
			// Get mesh name
			aiMesh* mesh = scene->mMeshes[curMeshIndex];
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
				myMesh.mVertices.push_back(vertex);
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
						myMesh.mIndices.push_back(mesh->mFaces[j].mIndices[k]);
					}
				}

				// Else find out how many indices there are per face
				else
				{
					for (UINT k = 0; k < mesh->mFaces[j].mNumIndices; ++k)
					{
						// Insert indices to model
						myMesh.mIndices.push_back(mesh->mFaces[j].mIndices[k]);
					}
				}

			}

			// Update total vertex and face count
			totalVertexCount += numVertices;
			totalFaceCount += numFaces;

			myMesh.mMaterialIndex = mesh->mMaterialIndex;
			myMesh.mNumFaces = numFaces;
			myMesh.mNumVertices = numVertices;
			myMesh.mName = materials[myMesh.mMaterialIndex].name;

			//------------------------------------------------------------
			// Read bones
			//------------------------------------------------------------
			if (mesh->HasBones())
			{
				ReadBones(mesh, myMesh);

				// Set vertex weights
				SetVertexWeights(myMesh);
			}

			meshes.push_back(myMesh);
		} // Mesh end

	} // If scene contains mesh end

	// Clean up after reading file
	DefaultLogger::kill();

	return true;
}

bool GenericObjectLoader::loadSkinnedObject( const std::string& fileName, std::vector<GenericMaterial>& materials,
	std::vector<GenericSkinnedMesh>& meshes, SkinnedData& skinnedData )
{
	using namespace Assimp;

	// Create instance of assimp Importer class
	Importer importer;

	std::string logName = "Logs\\AssimpMeshLoaderSKINNED.log";

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

		// Load skeleton
		skinnedData.Skeleton = CreateBoneTree(scene->mRootNode, NULL, skinnedData);

		// Read animations
		ReadAnimations(scene, skinnedData);

		std::vector<Vertex::PosNormalTexTanSkinned*> VertexList;

		//------------------------------------------------------------
		// Read materials
		//------------------------------------------------------------
		ReadMaterials(scene, materials);

		//----------------------------------------------------------
		// Read all scene meshes
		//----------------------------------------------------------
		for (UINT curMeshIndex = 0; curMeshIndex < scene->mNumMeshes; ++curMeshIndex)
		{
			// Get mesh name
			aiMesh* mesh = scene->mMeshes[curMeshIndex];
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
				myMesh.mVertices.push_back(vertex);
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
						myMesh.mIndices.push_back(mesh->mFaces[j].mIndices[k]);
					}
				}

				// Else find out how many indices there are per face
				else
				{
					for (UINT k = 0; k < mesh->mFaces[j].mNumIndices; ++k)
					{
						// Insert indices to model
						myMesh.mIndices.push_back(mesh->mFaces[j].mIndices[k]);
					}
				}

			}

			// Update total vertex and face count
			totalVertexCount += numVertices;
			totalFaceCount += numFaces;

			myMesh.mMaterialIndex = mesh->mMaterialIndex;
			myMesh.mNumFaces = numFaces;
			myMesh.mNumVertices = numVertices;
			myMesh.mName = materials[myMesh.mMaterialIndex].name;

			//------------------------------------------------------------
			// Read bones
			//------------------------------------------------------------
// 			if (mesh->HasBones())
// 			{
// 				ReadBones(mesh, myMesh);
// 
// 				// Set vertex weights
 				//SetVertexWeights(myMesh);
// 			}

			for (UINT i = 0; i < mesh->mNumBones; ++i)
			{
				const aiBone* bone = mesh->mBones[i];

				std::map<std::string, SkinData::Bone*>::iterator found = skinnedData.BonesByName.find(bone->mName.data);

				// Bone was found
				if (found != skinnedData.BonesByName.end())
				{
					bool skipAdd = false;
					for (UINT j = 0; j < skinnedData.Bones.size(); ++j)
					{
						std::string boneName = bone->mName.data;
						if (skinnedData.Bones[j]->Name == boneName)
						{
							skipAdd = true;
							break;
						}
					}

					if (!skipAdd)
					{
						std::string tes = found->second->Name;
						SkinData::ReadAiMatrix(found->second->Offset, bone->mOffsetMatrix);

						XMMATRIX offsetMatrix = XMLoadFloat4x4(&found->second->Offset);
						offsetMatrix = XMMatrixTranspose(offsetMatrix);
						XMStoreFloat4x4(&found->second->Offset, offsetMatrix);

						skinnedData.Bones.push_back(found->second);
						skinnedData.BonesToIndex[found->first] = (UINT)skinnedData.Bones.size()-1;
					}
				}

				for (UINT j = 0; j < bone->mNumWeights; ++j)
				{
					if (myMesh.mVertices[bone->mWeights[j].mVertexId].weights.x == 0.0f)
					{
						myMesh.mVertices[bone->mWeights[j].mVertexId].boneIndices[0] = (UINT)skinnedData.Bones.size()-1;
						myMesh.mVertices[bone->mWeights[j].mVertexId].weights.x = bone->mWeights[j].mWeight;
					}

					else if (myMesh.mVertices[bone->mWeights[j].mVertexId].weights.y == 0.0f)
					{
						myMesh.mVertices[bone->mWeights[j].mVertexId].boneIndices[1] = (UINT)skinnedData.Bones.size()-1;
						myMesh.mVertices[bone->mWeights[j].mVertexId].weights.y = bone->mWeights[j].mWeight;
					}

					else if (myMesh.mVertices[bone->mWeights[j].mVertexId].weights.z == 0.0f)
					{
						myMesh.mVertices[bone->mWeights[j].mVertexId].boneIndices[2] = (UINT)skinnedData.Bones.size()-1;
						myMesh.mVertices[bone->mWeights[j].mVertexId].weights.z = bone->mWeights[j].mWeight;
					}
					else
					{
						myMesh.mVertices[bone->mWeights[j].mVertexId].boneIndices[3] = (UINT)skinnedData.Bones.size()-1;
						// This weight will be figured out in the vertex shader by taking 1 - the sum of the 3 other weights
					}
				}
			}

			meshes.push_back(myMesh);
		} // Mesh end

		skinnedData.Transforms.resize(skinnedData.Bones.size());
		float timeStep = 1.0f/30.0f;

		// Pre-calculate animations
		for (UINT i = 0; i < skinnedData.Animations.size(); ++i)
		{
			skinnedData.SetAnimIndex(i);
			float dt = 0;
			for (float ticks = 0; ticks < skinnedData.Animations[i].mDuration; ticks += skinnedData.Animations[i].mTicksPerSecond/30.0f)
			{
				dt += timeStep;
				skinnedData.Calculate(dt);
				skinnedData.Animations[i].Transforms.push_back(std::vector<XMFLOAT4X4>());
				std::vector<XMFLOAT4X4>& trans = skinnedData.Animations[i].Transforms.back();

				for (UINT j = 0; j < skinnedData.Transforms.size(); ++j)
				{
					XMMATRIX offsetMatrix = XMLoadFloat4x4(&skinnedData.Bones[j]->Offset);
					XMMATRIX globalTransform = XMLoadFloat4x4(&skinnedData.Bones[j]->GlobalTransform);
					XMMATRIX rotationMat = XMMatrixMultiply(offsetMatrix, globalTransform);

					XMFLOAT4X4 rotationMat4x4;
					XMStoreFloat4x4(&rotationMat4x4, rotationMat);

					trans.push_back(rotationMat4x4);
				}
			}
		}

	} // If scene contains mesh end

	// Clean up after reading file
	DefaultLogger::kill();

	return true;
}

//=================================================================================
// Materials
//=================================================================================
void GenericObjectLoader::ReadMaterials(const aiScene* scene,
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
			InitMaterial(&genMat);

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

void GenericObjectLoader::InitMaterial(GenericMaterial* material)
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
// Bones
//=================================================================================
/*
void GenericObjectLoader::ReadBoneKeyframes(aiNodeAnim* nodeAnim,
							BoneAnimation& boneAnimation)
{
	UINT numPosKeys = nodeAnim->mNumPositionKeys;
	UINT numScaleKeys = nodeAnim->mNumScalingKeys;
	UINT numRotKeys = nodeAnim->mNumRotationKeys;

	// Find which type of key has the most values
	UINT* largestKeyType;
	largestKeyType = MathHelper::getMax(&numPosKeys, &numScaleKeys);
	largestKeyType = MathHelper::getMax(largestKeyType, &numRotKeys);

	// And set our keyframe vector to that size
	boneAnimation.keyFrames.resize(*largestKeyType);

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
*/

void GenericObjectLoader::ReadBones(aiMesh* mesh, GenericSkinnedMesh& myMesh)
{
	for (UINT i = 0; i < mesh->mNumBones; ++i)				
	{
		aiBone* aibone = mesh->mBones[i];

		SkinData::Bone bone;
		bone.Name = aibone->mName.data;
		bone.Offset = XMFLOAT4X4(
			aibone->mOffsetMatrix.a1, aibone->mOffsetMatrix.a2, aibone->mOffsetMatrix.a3, aibone->mOffsetMatrix.a4,
			aibone->mOffsetMatrix.b1, aibone->mOffsetMatrix.b2, aibone->mOffsetMatrix.b3, aibone->mOffsetMatrix.b4,
			aibone->mOffsetMatrix.c1, aibone->mOffsetMatrix.c2, aibone->mOffsetMatrix.c3, aibone->mOffsetMatrix.c4,
			aibone->mOffsetMatrix.d1, aibone->mOffsetMatrix.d2, aibone->mOffsetMatrix.d3, aibone->mOffsetMatrix.d4
			);

 		for (UINT j = 0; j < aibone->mNumWeights; ++j)
		{
			SkinData::VertexWeight vertexWeight;
			vertexWeight.VertexID = aibone->mWeights[j].mVertexId;
			vertexWeight.Weight = aibone->mWeights[j].mWeight;

			bone.Weights.push_back(vertexWeight);
		}

		myMesh.mBones.push_back(bone);
	}
}


void GenericObjectLoader::SetVertexWeights(GenericSkinnedMesh& myMesh)
{
	for (UINT i = 0; i < myMesh.mBones.size(); ++i)
	{
		for (UINT j = 0; j < myMesh.mBones[i].Weights.size(); ++j)
		{
			UINT VertexID = myMesh.mBones[i].Weights[j].VertexID;
			float Weight = myMesh.mBones[i].Weights[j].Weight;

			if (myMesh.mVertices[VertexID].weights.x == 0.0f)
			{
				myMesh.mVertices[VertexID].weights.x = Weight;
				myMesh.mVertices[VertexID].boneIndices[0] = i;
			}
				else if (myMesh.mVertices[VertexID].weights.y == 0.0f)
			{
				myMesh.mVertices[VertexID].weights.y = Weight;
				myMesh.mVertices[VertexID].boneIndices[1] = i;
			}
				else if (myMesh.mVertices[VertexID].weights.z == 0.0f)
			{
				myMesh.mVertices[VertexID].weights.z = Weight;
				myMesh.mVertices[VertexID].boneIndices[2] = i;
			}
		}
	}
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

	std::string logName = "Logs\\AssimpMeshLoader.log";

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

		//------------------------------------------------------------
		// Read materials
		//------------------------------------------------------------
		ReadMaterials(scene, materials);

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

//======================================================================
// Recursively creates a bone tree
//======================================================================
SkinData::Bone* GenericObjectLoader::CreateBoneTree(aiNode* node,
										SkinData::Bone* parent, SkinnedData& skinnedData)
{
	SkinData::Bone* internalNode = new SkinData::Bone();
	internalNode->Name = node->mName.data;
	internalNode->Parent = parent; // Set parent, if bone is root parent == NULL

	skinnedData.BonesByName[internalNode->Name] = internalNode;

	// Bone bone local transform matrix
	SkinData::ReadAiMatrix(internalNode->LocalTransform, node->mTransformation);

	// Transpose local transform matrix
	XMMATRIX LocalTransformTranspose = XMLoadFloat4x4(&internalNode->LocalTransform);
	LocalTransformTranspose = XMMatrixTranspose(LocalTransformTranspose);
	XMStoreFloat4x4(&internalNode->LocalTransform, LocalTransformTranspose);
	internalNode->OriginalLocalTransform = internalNode->LocalTransform;

	// Calculate bone to world transform matrix
	SkinData::CalculateBoneToWorldTransform(internalNode);

	// Continue for all child nodes
	for (UINT i = 0; i < node->mNumChildren; ++i)
	{
		internalNode->Children.push_back(CreateBoneTree(node->mChildren[i], internalNode, skinnedData));
	}

	return internalNode;
}

// void GenericObjectLoader::CalculateBoneToWorldTransform(SkinData::Bone* child)
// {
// 	child->GlobalTransform = child->LocalTransform;
// 	XMMATRIX Child_GlobalTransform = XMLoadFloat4x4(&child->GlobalTransform);
// 
// 	SkinData::Bone* parent = child->Parent;
// 
// 	// Climb up towards the root bone
// 	while (parent)
// 	{
// 		XMMATRIX Parent_LocalTransform = XMLoadFloat4x4(&parent->LocalTransform);
// 
// 		XMMatrixMultiply(Child_GlobalTransform, Parent_LocalTransform);
// 
// 		//child->GlobalTransform *= parent->LocalTransform;
// 		parent = parent->Parent;
// 	}
// 
// 	// The final to world transform has been calculated, store it in child's GlobalTransform
// 	XMStoreFloat4x4(&child->GlobalTransform, Child_GlobalTransform);
// }

void GenericObjectLoader::ReadAnimations( const aiScene* scene, SkinnedData& skinnedData )
{
	for (UINT i = 0; i < scene->mNumAnimations; ++i)
	{
		skinnedData.Animations.push_back(AnimEvaluator(scene->mAnimations[i]));
	}

	// Get all the animation names, so we'll be able to reference them by name and get their ID
	for (UINT i = 0; i < skinnedData.Animations.size(); ++i)
		skinnedData.AnimationNameToId.insert(std::map<std::string, UINT>::value_type(skinnedData.Animations[i].Name, i));

	skinnedData.CurrentAnimIndex = 0;
	skinnedData.SetAnimation("Idle");
}
