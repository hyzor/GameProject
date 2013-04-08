#include <d3dUtilities.h>
#include <D3D11App.h>
#include <Camera.h>
#include <TextureManager.h>
#include "BasicModel.h"
#include "ShadowMap.h"
#include "Terrain.h"
#include "ParticleSystem.h"
//#include "Ssao.h"
#include "Sky.h"
#include <xnacollision.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "GenericModel.h"
#include "GenericSkinnedModel.h"

#include "GameSettings.h"

#include "Player.h"

// API for xbox 360 controller
//#include <XInput.h>

struct BoundingSphere
{
	BoundingSphere() : Center(0.0f, 0.0f, 0.0f), Radius(0.0f) {}
	XMFLOAT3 Center;
	float Radius;
};

class Projekt : public D3D11App
{
public:
	Projekt(HINSTANCE hInstance);
	~Projekt();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	// WIN32
	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	void buildShadowTransform();
	void drawSceneToShadowMap();

	// Direct Input
// 	bool InitDirectInput(HINSTANCE hInstance);
// 	void DetectInput(double time);

private:
	//Camera mCam;
	TextureManager mTextureMgr;
	Terrain mTerrain;

	ParticleSystem mFire;

	//Ssao* mSsao;

	Sky* mSky;

	ID3D11ShaderResourceView* mFlareTexSRV;
	ID3D11ShaderResourceView* mRandomTexSRV;

	// Models
	GenericModel* mGenericModel;

	GenericModel* mPlayerModel;

	std::vector<GenericModelInstance> mGenericInstances;

	// Skinned models
	GenericSkinnedModel* mSkinnedModel;
	std::vector<GenericSkinnedModelInstance> mGenSkinnedInstances;

	// Lights
	PointLight mPointLight;
	XMFLOAT3 mOriginalLightDir[3];
	DirectionalLight mDirLights[3];

	// Shadow map
	UINT mShadowMapSize;
	ShadowMap* mShadowMap;
	XMFLOAT4X4 mLightView;
	XMFLOAT4X4 mLightProj;
	XMFLOAT4X4 mShadowTransform;

	BoundingSphere mSceneBounds;

	bool mWalkCamMode;

	POINT mLastMousePos;

	// Camera frustum
	XNA::Frustum mCamFrustum;

	UINT mVisibleObjectCount;
	bool mFrustumCullingEnabled;

	// Fogging
	FogSettings mFogSettings;

	// Render states
	ID3D11RasterizerState* WireFrameRS;

	//GameSettings mGameSettings;

	Player mPlayer;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	// Also create a debug console window
	// 	if(AllocConsole()) 
	// 	{
	// 		freopen("CONOUT$", "w", stdout);
	// 		SetConsoleTitle(L"Debug Console");
	// 		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);  
	// 	}

#endif

	Projekt theApp(hInstance);

	if (!theApp.Init())
		return 0;

	return theApp.Run();
}

Projekt::Projekt(HINSTANCE hInstance)
	: D3D11App(hInstance), mShadowMap(0), mWalkCamMode(false),
	mVisibleObjectCount(0), mFrustumCullingEnabled(true)
	//mSsao(0)
{
	mMainWndCaption = L"DV1415 - Projekt";

	//mPlayer.GetCamera()->setPosition(0.0f, 60.0f, -125.0f);

	//--------------------------------------------------------
	// Create lights
	//--------------------------------------------------------
	mDirLights[0].Ambient  = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mDirLights[0].Diffuse  = XMFLOAT4(0.8f, 0.7f, 0.7f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.6f, 0.6f, 0.7f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	mDirLights[1].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse  = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	mDirLights[2].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse  = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(0.0f, 0.0, -1.0f);

	mOriginalLightDir[0] = mDirLights[0].Direction;
	mOriginalLightDir[1] = mDirLights[1].Direction;
	mOriginalLightDir[2] = mDirLights[2].Direction;
}

Projekt::~Projekt()
{
	SafeDelete(mGenericModel);

	//SafeDelete(mSkinnedModel);

	SafeDelete(mPlayerModel);

	SafeDelete(mSky);

	SafeDelete(mShadowMap);

	ReleaseCOM(WireFrameRS);

	Effects::DestroyAll();
	InputLayouts::DestroyAll();

	delete GameSettings::Instance();
}

bool Projekt::Init()
{
	if (!D3D11App::Init())
		return false;

	// Initialize effects, input layouts and texture manager
	Effects::InitAll(mDirect3D->GetDevice());
	InputLayouts::InitAll(mDirect3D->GetDevice());
	mTextureMgr.init(mDirect3D->GetDevice());

	// Initialize wireframe render state
	D3D11_RASTERIZER_DESC wireFrameDesc;
	ZeroMemory(&wireFrameDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireFrameDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireFrameDesc.CullMode = D3D11_CULL_BACK;
	wireFrameDesc.FrontCounterClockwise = false;
	wireFrameDesc.DepthClipEnable = true;

	HR(mDirect3D->GetDevice()->CreateRasterizerState(&wireFrameDesc, &WireFrameRS));

	//--------------------------------------------------------
	// Create sky
	//--------------------------------------------------------
	mSky = new Sky(mDirect3D->GetDevice(), L"Data/Textures/snowcube1024.dds", 5000.0f);

	//--------------------------------------------------------
	// Create terrain
	//--------------------------------------------------------
	// Describe terrain
	Terrain::InitInfo tInitInfo;
	tInitInfo.HeightMapFilename = L"Data/Textures/Heightmaps/terrain.raw";
	tInitInfo.LayerMapFilename0 = L"Data/Textures/grass.dds";
	tInitInfo.LayerMapFilename1 = L"Data/Textures/darkdirt.dds";
	tInitInfo.LayerMapFilename2 = L"Data/Textures/stone.dds";
	tInitInfo.LayerMapFilename3 = L"Data/Textures/lightdirt.dds";
	tInitInfo.LayerMapFilename4 = L"Data/Textures/snow.dds";
	tInitInfo.BlendMapFilename = L"Data/Textures/blend.dds";
	tInitInfo.HeightScale = 50.0f;
	tInitInfo.HeightmapWidth = 2049;
	tInitInfo.HeightmapHeight = 2049;
	tInitInfo.CellSpacing = 0.5f;

	// Initialize terrain
	mTerrain.Init(mDirect3D->GetDevice(), mDirect3D->GetImmediateContext(), tInitInfo);

	//--------------------------------------------------------
	// Particle systems
	//--------------------------------------------------------
	mRandomTexSRV = d3dHelper::CreateRandomTexture1DSRV(mDirect3D->GetDevice());

	std::vector<std::wstring> flares;
	flares.push_back(L"Data\\Textures\\flare0.dds");
	mFlareTexSRV = d3dHelper::CreateTexture2DArraySRV(mDirect3D->GetDevice(), mDirect3D->GetImmediateContext(), flares);

	mFire.init(mDirect3D->GetDevice(), Effects::FireFX, mFlareTexSRV, mRandomTexSRV, 500);
	mFire.setEmitPos(XMFLOAT3(65.0f, 5.0f, 0.0f));

	//--------------------------------------------------------
	// Create shadow map
	//--------------------------------------------------------
	mShadowMapSize = 2048;
	mShadowMap = new ShadowMap(mDirect3D->GetDevice(), mShadowMapSize, mShadowMapSize);

	//--------------------------------------------------------
	// Load models
	//--------------------------------------------------------
	mGenericModel = new GenericModel(mDirect3D->GetDevice(), mTextureMgr, "Data\\Models\\Collada\\duck.dae", L"Data\\Models\\Collada\\");

	//mSkinnedModel = new GenericSkinnedModel(mDirect3D->GetDevice(), mTextureMgr, "Data\\Models\\Collada\\AnimTest\\test_Collada_DAE.dae", L"Data\\Models\\Collada\\AnimTest\\");

	mPlayerModel = new GenericModel(mDirect3D->GetDevice(), mTextureMgr, "Data\\Models\\OBJ\\Cop\\cop.obj", L"Data\\Models\\OBJ\\Cop\\");

	Player::InitProperties playerProp;

	playerProp.PlayerID = 0;
	playerProp.Nickname = "Hyzor";
	playerProp.Speed = 1.0f;
	playerProp.Health = 1.0f;
	playerProp.Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	playerProp.Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	playerProp.Angle = 0.0f;
	playerProp.Model = mPlayerModel;

	mPlayer.Init(playerProp);
	
	//--------------------------------------------------------
	// Create model instances
	//--------------------------------------------------------

	GenericModelInstance genericInstance;
	genericInstance.model = mGenericModel;

// 	GenericSkinnedModelInstance genSkinnedInstance;
// 	genSkinnedInstance.model = mSkinnedModel;
// 	genSkinnedInstance.FinalTransforms.resize(mSkinnedModel->skinnedData.getBoneCount());
// 	genSkinnedInstance.ClipName = "animation";
// 	genSkinnedInstance.TimePos = 0.0f;

	//--------------------------------------------------------
	// Scale, rotate and move model instances
	//--------------------------------------------------------
	XMMATRIX modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX modelRot = XMMatrixRotationY(0.0f);
	XMMATRIX modelOffset = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	//modelScale = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	modelOffset = XMMatrixTranslation(-30.0f, 15.0f, -110.0f);
	XMStoreFloat4x4(&genericInstance.world, modelScale*modelRot*modelOffset);

// 	modelOffset = XMMatrixTranslation(50.0f, 15.0f, -110.0f);
// 	XMStoreFloat4x4(&genSkinnedInstance.world, modelScale*modelRot*modelOffset);

	//--------------------------------------------------------
	// Insert model instances to the vector
	//--------------------------------------------------------
	mGenericInstances.push_back(genericInstance);

/*	mGenSkinnedInstances.push_back(genSkinnedInstance);*/

	//--------------------------------------------------------
	// Compute scene bounding box
	//--------------------------------------------------------
	XMFLOAT3 minPt(+MathHelper::infinity, +MathHelper::infinity, +MathHelper::infinity);
	XMFLOAT3 maxPt(-MathHelper::infinity, -MathHelper::infinity, -MathHelper::infinity);

	// Get vertex positions from all models
	for (UINT i = 0; i < mGenericInstances.size(); ++i)
	{
		for (UINT j = 0; j < mGenericInstances[i].model->vertices.size(); ++j)
		{
			XMFLOAT3 vPos = mGenericInstances[i].model->vertices[j]->position;

			minPt.x = MathHelper::getMin(minPt.x, vPos.x);
			minPt.y = MathHelper::getMin(minPt.x, vPos.x);
			minPt.z = MathHelper::getMin(minPt.x, vPos.x);

			maxPt.x = MathHelper::getMax(maxPt.x, vPos.x);
			maxPt.y = MathHelper::getMax(maxPt.x, vPos.x);
			maxPt.z = MathHelper::getMax(maxPt.x, vPos.x);
		}
	}

	// Get vertex positions from all skinned models
// 	for (UINT i = 0; i < mGenSkinnedInstances.size(); ++i)
// 	{
// 		for (UINT j = 0; j < mGenSkinnedInstances[i].model->vertices.size(); ++j)
// 		{
// 			XMFLOAT3 vPos = mGenSkinnedInstances[i].model->vertices[j]->position;
// 
// 			minPt.x = MathHelper::getMin(minPt.x, vPos.x);
// 			minPt.y = MathHelper::getMin(minPt.x, vPos.x);
// 			minPt.z = MathHelper::getMin(minPt.x, vPos.x);
// 
// 			maxPt.x = MathHelper::getMax(maxPt.x, vPos.x);
// 			maxPt.y = MathHelper::getMax(maxPt.x, vPos.x);
// 			maxPt.z = MathHelper::getMax(maxPt.x, vPos.x);
// 		}
// 	}

	// Now continue with terrain vertex positions
	for (UINT i = 0; i < mTerrain.getPatchVertices().size(); ++i)
	{
		XMFLOAT3 vPos = mTerrain.getPatchVertices()[i].position;

		minPt.x = MathHelper::getMin(minPt.x, vPos.x);
		minPt.y = MathHelper::getMin(minPt.x, vPos.x);
		minPt.z = MathHelper::getMin(minPt.x, vPos.x);

		maxPt.x = MathHelper::getMax(maxPt.x, vPos.x);
		maxPt.y = MathHelper::getMax(maxPt.x, vPos.x);
		maxPt.z = MathHelper::getMax(maxPt.x, vPos.x);
	}

	// Sphere center is at half of these new dimensions
	mSceneBounds.Center = XMFLOAT3(	0.5f*(minPt.x + maxPt.x),
		0.5f*(minPt.y + maxPt.y),
		0.5f*(minPt.z + maxPt.z));

	// Calculate the sphere radius
	XMFLOAT3 extent(0.5f*(maxPt.x - minPt.x),
		0.5f*(maxPt.y - minPt.y),
		0.5f*(maxPt.z - minPt.z));

	mSceneBounds.Radius = sqrtf(extent.x*extent.x + extent.y*extent.y + extent.z*extent.z);

	OnResize();

	return true;
}

void Projekt::OnResize()
{
	D3D11App::OnResize();

	//mPlayer.GetCamera()->setLens(0.25f*MathHelper::pi, AspectRatio(), 1.0f, 1000.0f);

	mPlayer.GetCamera()->setLens(0.25f*MathHelper::pi, AspectRatio(), 1.0f, 1000.0f);

	// Build the frustum from the projection matrix in view space
	ComputeFrustumFromProjection(&mCamFrustum, &mPlayer.GetCamera()->getProjMatrix());
}

void Projekt::DrawScene()
{
	//---------------------------------------------------------------------------
	// Render scene to shadow map
	//---------------------------------------------------------------------------
	mShadowMap->BindDsvAndSetNullRenderTarget(mDirect3D->GetImmediateContext());
	drawSceneToShadowMap();

	mDirect3D->GetImmediateContext()->RSSetState(0);

	// Restore back and depth buffer and viewport to the OM stage
	ID3D11RenderTargetView* renderTargets[1] = {mDirect3D->GetRenderTargetView()};
	mDirect3D->GetImmediateContext()->OMSetRenderTargets(1, renderTargets, mDirect3D->GetDepthStencilView());
	mDirect3D->GetImmediateContext()->ClearRenderTargetView(mDirect3D->GetRenderTargetView(), reinterpret_cast<const float*>(&Colors::LightSteelBlue));

	mDirect3D->GetImmediateContext()->ClearDepthStencilView(mDirect3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	mDirect3D->GetImmediateContext()->RSSetViewports(1, &mDirect3D->GetScreenViewport());

	// Possible Wireframe render state
	if (GetAsyncKeyState('E') & 0x8000)
		mDirect3D->GetImmediateContext()->RSSetState(WireFrameRS);

	XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowTransform);

	//---------------------------------------------------------------------------
	// Draw terrain
	//---------------------------------------------------------------------------
	//mTerrain.Draw(mDirect3D->GetImmediateContext(), mCam, mDirLights);

	mTerrain.DrawShadowed(mDirect3D->GetImmediateContext(), *mPlayer.GetCamera(), mDirLights,
		mShadowMap->getDepthMapSRV(), &shadowTransform);

	// --------------------------------------------------------------------------

	// Camera matrices
	XMMATRIX view = mPlayer.GetCamera()->getViewMatrix();
	XMMATRIX proj = mPlayer.GetCamera()->getProjMatrix();
	XMMATRIX viewproj = mPlayer.GetCamera()->getViewProjMatrix();

	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};

	// Set per frame constants
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(mPlayer.GetCamera()->getPosition());
	Effects::BasicFX->setShadowMap(mShadowMap->getDepthMapSRV());
	Effects::BasicFX->SetCubeMap(mSky->cubeMapSRV());

	Effects::NormalMapFX->SetDirLights(mDirLights);
	Effects::NormalMapFX->SetEyePosW(mPlayer.GetCamera()->getPosition());
	Effects::NormalMapFX->setShadowMap(mShadowMap->getDepthMapSRV());
	Effects::NormalMapFX->SetCubeMap(mSky->cubeMapSRV());

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	// Vertex size & offset
	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	// Draw player
	//mPlayer.Draw(mDirect3D->GetImmediateContext(), mDirLights,
		//mShadowMap->getDepthMapSRV(), &shadowTransform);

	//---------------------------------------------------------------------------
	// Draw opaque objects
	//---------------------------------------------------------------------------
	// Bind information about primitive type, and set input layout
	mDirect3D->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mDirect3D->GetImmediateContext()->IASetInputLayout(InputLayouts::Basic32);

	// Set our effect technique to use
	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->DirLights3FogTexTech;
	ID3DX11EffectTechnique* activeSkinnedTech = Effects::NormalMapFX->DirLights3TexTech;

	D3DX11_TECHNIQUE_DESC techDesc;

	//--------------------------------------------------------------------------------
	// Draw opaque tessellated objects
	//--------------------------------------------------------------------------------
	Effects::BasicTessFX->SetDirLights(mDirLights);
	Effects::BasicTessFX->SetEyePosW(mPlayer.GetCamera()->getPosition());
	Effects::BasicTessFX->setShadowMap(mShadowMap->getDepthMapSRV());
	Effects::BasicTessFX->SetCubeMap(mSky->cubeMapSRV());

	mDirect3D->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	mDirect3D->GetImmediateContext()->IASetInputLayout(InputLayouts::PosNormalTexTan);

	activeTech = Effects::BasicTessFX->TessDirLights3FogTexTech;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		for (UINT mIndex = 0; mIndex < mGenericInstances.size(); ++mIndex)
		{
			if (mGenericInstances[mIndex].isVisible)
			{
				world = XMLoadFloat4x4(&mGenericInstances[mIndex].world);
				worldInvTranspose = MathHelper::InverseTranspose(world);
				worldViewProj = world*view*proj;

				Effects::BasicTessFX->SetWorld(world);
				Effects::BasicTessFX->SetWorldInvTranspose(worldInvTranspose);
				Effects::BasicTessFX->SetWorldViewProj(worldViewProj);
				Effects::BasicTessFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
				Effects::BasicTessFX->setShadowTransform(world*shadowTransform);
				Effects::BasicTessFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
				Effects::BasicTessFX->SetMinTessDistance(20.0f);
				Effects::BasicTessFX->SetMaxTessDistance(200.0f);
				Effects::BasicTessFX->SetMinTessFactor(0.0f);
				Effects::BasicTessFX->SetMaxTessFactor(3.0f);
				Effects::BasicTessFX->setFogStart(GameSettings::Instance()->Fog()->fogStart);
				Effects::BasicTessFX->setFogRange(GameSettings::Instance()->Fog()->fogRange);

				
				Effects::BasicTessFX->setFogColor(Colors::Silver);

				for (UINT i = 0; i < mGenericInstances[mIndex].model->meshCount; ++i)
				{
					UINT matIndex = mGenericInstances[mIndex].model->meshes[i].MaterialIndex;

					Effects::BasicTessFX->SetMaterial(mGenericInstances[mIndex].model->mat[matIndex]);

					Effects::BasicTessFX->SetDiffuseMap(mGenericInstances[mIndex].model->diffuseMapSRV[matIndex]);
					//Effects::BasicTessFX->SetNormalMap(mGenericInstances[mIndex].model->normalMapSRV[matIndex]);

					activeTech->GetPassByIndex(p)->Apply(0, mDirect3D->GetImmediateContext());
					mGenericInstances[mIndex].model->meshes[i].draw(mDirect3D->GetImmediateContext());
				}
			}
		}
	}

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	mDirect3D->GetImmediateContext()->HSSetShader(0, 0, 0);
	mDirect3D->GetImmediateContext()->DSSetShader(0, 0, 0);

	mDirect3D->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mDirect3D->GetImmediateContext()->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);

	// Skinned objects
	activeSkinnedTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		for (UINT mIndex = 0; mIndex < mGenSkinnedInstances.size(); ++mIndex)
		{
			if (mGenSkinnedInstances[mIndex].isVisible)
			{
				world = XMLoadFloat4x4(&mGenSkinnedInstances[mIndex].world);
				worldInvTranspose = MathHelper::InverseTranspose(world);
				worldViewProj = world*view*proj;

				Effects::NormalMapFX->SetWorld(world);
				Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
				Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
				Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
				Effects::NormalMapFX->setShadowTransform(world*shadowTransform);
				Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
				Effects::NormalMapFX->setFogStart(GameSettings::Instance()->Fog()->fogStart);
				Effects::NormalMapFX->setFogRange(GameSettings::Instance()->Fog()->fogRange);
				Effects::NormalMapFX->setBoneTransforms(&mGenSkinnedInstances[mIndex].FinalTransforms[0],
					mGenSkinnedInstances[mIndex].FinalTransforms.size());


				Effects::BasicTessFX->setFogColor(Colors::Silver);

				for (UINT i = 0; i < mGenSkinnedInstances[mIndex].model->numMeshes; ++i)
				{
					UINT matIndex = mGenSkinnedInstances[mIndex].model->meshes[i].MaterialIndex;

					Effects::NormalMapFX->SetMaterial(mGenSkinnedInstances[mIndex].model->mat[matIndex]);

					Effects::NormalMapFX->SetDiffuseMap(mGenSkinnedInstances[mIndex].model->diffuseMapSRV[matIndex]);

					Effects::NormalMapFX->setNormalMap(mGenSkinnedInstances[mIndex].model->normalMapSRV[matIndex]);

					activeSkinnedTech->GetPassByIndex(p)->Apply(0, mDirect3D->GetImmediateContext());
					mGenSkinnedInstances[mIndex].model->meshes[i].draw(mDirect3D->GetImmediateContext());
				}
			}
		}
	}

	//---------------------------------------------------------------------------
	// Draw particle systems
	//---------------------------------------------------------------------------
	mFire.setEyePos(mPlayer.GetCamera()->getPosition());
	mFire.draw(mDirect3D->GetImmediateContext(), *mPlayer.GetCamera());
	mDirect3D->GetImmediateContext()->OMSetBlendState(0, blendFactor, 0xffffffff); // restore default

	//---------------------------------------------------------------------------
	// Draw sky
	//---------------------------------------------------------------------------
	mSky->draw(mDirect3D->GetImmediateContext(), *mPlayer.GetCamera());

	// Unbind shadow map and AmbientMap as a shader input because we are going to render
	// to it next frame.  These textures can be at any slot, so clear all slots.
	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	mDirect3D->GetImmediateContext()->PSSetShaderResources(0, 16, nullSRV);

	// Restore default states
	mDirect3D->GetImmediateContext()->RSSetState(0);
	mDirect3D->GetImmediateContext()->OMSetDepthStencilState(0, 0);
	mDirect3D->GetImmediateContext()->OMSetBlendState(0, blendFactor, 0xffffffff); 

	HR(mDirect3D->GetSwapChain()->Present(0, 0));
}

void Projekt::UpdateScene(float dt)
{
	mPlayer.Update(dt, mDirectInput);

	for (UINT i = 0; i < mGenSkinnedInstances.size(); ++i)
		mGenSkinnedInstances[i].Update(dt);

	/*
	// Up
	if (mDirectInput->GetKeyboardState()[DIK_W] && 0x80)
	{
		mPlayer.GetCamera()->walk(30.0f*dt);
	}

	// Left
	if (mDirectInput->GetKeyboardState()[DIK_A] & 0x80)
	{
		mPlayer.GetCamera()->strafe(-30.0f*dt);
	}

	// Down
	if (mDirectInput->GetKeyboardState()[DIK_S] & 0x80)
	{
		mPlayer.GetCamera()->walk(-30.0f*dt);
	}

	// Right
	if (mDirectInput->GetKeyboardState()[DIK_D] & 0x80)
	{
		mPlayer.GetCamera()->strafe(30.0f*dt);
	}

	// Mouse has moved in x-axis
	if (mDirectInput->MouseHasMoved())
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(mDirectInput->GetMouseState().lX));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(mDirectInput->GetMouseState().lY));

		mPlayer.GetCamera()->yaw(dx);
		mPlayer.GetCamera()->pitch(dy);
	}
	*/

	//DetectInput(dt);
	// Movement
// 	if(GetAsyncKeyState('W') & 0x8000)
// 		mPlayer.GetCamera()->walk(30.0f*dt);
// 
// 	if(GetAsyncKeyState('S') & 0x8000)
// 		mPlayer.GetCamera()->walk(-30.0f*dt);
// 
// 	if(GetAsyncKeyState('A') & 0x8000)
// 		mPlayer.GetCamera()->strafe(-30.0f*dt);
// 
// 	if(GetAsyncKeyState('D') & 0x8000)
// 		mPlayer.GetCamera()->strafe(30.0f*dt);

	// Change shadow map resolution
// 	if(GetAsyncKeyState('1') & 0x8000)
// 	{
// 		mShadowMapSize = 256;
// 		mShadowMap->setResolution(mDirect3D->GetDevice(), mShadowMapSize, mShadowMapSize);
// 	}
// 
// 	if(GetAsyncKeyState('2') & 0x8000)
// 	{
// 		mShadowMapSize = 512;
// 		mShadowMap->setResolution(mDirect3D->GetDevice(), mShadowMapSize, mShadowMapSize);
// 	}
// 
// 	if(GetAsyncKeyState('3') & 0x8000)
// 	{
// 		mShadowMapSize = 1024;
// 		mShadowMap->setResolution(mDirect3D->GetDevice(), mShadowMapSize, mShadowMapSize);
// 	}
// 
// 	if(GetAsyncKeyState('4') & 0x8000)
// 	{
// 		mShadowMapSize = 2048;
// 		mShadowMap->setResolution(mDirect3D->GetDevice(), mShadowMapSize, mShadowMapSize);
// 	}
// 
// 	if(GetAsyncKeyState('5') & 0x8000)
// 	{
// 		mShadowMapSize = 4096;
// 		mShadowMap->setResolution(mDirect3D->GetDevice(), mShadowMapSize, mShadowMapSize);
// 	}
// 
// 	if(GetAsyncKeyState('6') & 0x8000)
// 	{
// 		mShadowMapSize = 8192;
// 		mShadowMap->setResolution(mDirect3D->GetDevice(), mShadowMapSize, mShadowMapSize);
// 	}

	// Walk/fly mode
// 	if(GetAsyncKeyState('Z') & 0x8000)
// 		mWalkCamMode = true;
// 	if(GetAsyncKeyState('X') & 0x8000)
// 		mWalkCamMode = false;


	// Walk mode
	if (mWalkCamMode)
	{
		XMFLOAT3 camPos = mPlayer.GetCamera()->getPosition();
		float y = mTerrain.GetHeight(camPos.x, camPos.z);
		mPlayer.GetCamera()->setPosition(camPos.x, y + 2.0f, camPos.z);
	}

	// Update particle systems
	mFire.update(dt, mTimer.getTimeElapsedS());

	// Build shadow map transform
	buildShadowTransform();

	// Update camera
	mPlayer.GetCamera()->updateViewMatrix();

	//------------------------------------------------------------------
	// Frustum culling
	//------------------------------------------------------------------
	mVisibleObjectCount = 0;

	if (mFrustumCullingEnabled)
	{
		XMVECTOR detView = XMMatrixDeterminant(mPlayer.GetCamera()->getViewMatrix());
		XMMATRIX invView = XMMatrixInverse(&detView, mPlayer.GetCamera()->getViewMatrix());

		for (UINT i = 0; i < mGenericInstances.size(); ++i)
		{
			mGenericInstances[i].isVisible = false;

			XMMATRIX W = XMLoadFloat4x4(&mGenericInstances[i].world);
			XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

			// View space to the object's local space.
			XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

			// Decompose the matrix into its individual parts.
			XMVECTOR scale;
			XMVECTOR rotQuat;
			XMVECTOR translation;
			XMMatrixDecompose(&scale, &rotQuat, &translation, toLocal);

			// Transform the camera frustum from view space to the object's local space.
			XNA::Frustum localspaceFrustum;
			XNA::TransformFrustum(&localspaceFrustum, &mCamFrustum, XMVectorGetX(scale), rotQuat, translation);

			// Perform the box/frustum intersection test in local space.
			if(XNA::IntersectAxisAlignedBoxFrustum(&mGenericInstances[i].model->boundingBox, &localspaceFrustum) != 0)
			{
				// Write the instance data to dynamic VB of the visible objects.
				//dataView[mVisibleObjectCount++] = mInstancedData[i];
				mVisibleObjectCount++;
				mGenericInstances[i].isVisible = true;
			}
		}
	}
	else
	{
		for (UINT i = 0; i < mGenericInstances.size(); ++i)
		{
			mGenericInstances[i].isVisible = true;
			mVisibleObjectCount++;
		}
	}

	std::wostringstream outs;   
	outs.precision(6);
	outs << L"    " << mVisibleObjectCount << 
		L" objects visible out of " << mGenericInstances.size();
	mMainWndCaption = outs.str();
}

void Projekt::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void Projekt::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void Projekt::OnMouseMove(WPARAM btnState, int x, int y)
{
// 	if((btnState & MK_LBUTTON) != 0)
// 	{
// 		// Make each pixel correspond to a quarter of a degree.
// 		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
// 		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));
// 
// 		mPlayer.GetCamera()->pitch(dy);
// 		mPlayer.GetCamera()->rotateY(dx);
// 	}
}

void Projekt::buildShadowTransform()
{
	// Only first "main" light casts a shadow
	// So get light direction and position from first light
	XMVECTOR lightDir = XMLoadFloat3(&mDirLights[0].Direction);

	XMVECTOR lightPos = -2.0f*mSceneBounds.Radius*lightDir;

	XMVECTOR targetPos = XMLoadFloat3(&mSceneBounds.Center);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

	// Transform bounding sphere to light space
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

	// Orthogonal frustum in light space encloses scene
	float l = sphereCenterLS.x - mSceneBounds.Radius;
	float b = sphereCenterLS.y - mSceneBounds.Radius;
	float n = sphereCenterLS.z - mSceneBounds.Radius;
	float r = sphereCenterLS.x + mSceneBounds.Radius;
	float t = sphereCenterLS.y + mSceneBounds.Radius;
	float f = sphereCenterLS.z + mSceneBounds.Radius;
	XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = V*P*T;

	XMStoreFloat4x4(&mLightView, V);
	XMStoreFloat4x4(&mLightProj, P);
	XMStoreFloat4x4(&mShadowTransform, S);
}

void Projekt::drawSceneToShadowMap()
{
	XMMATRIX view = XMLoadFloat4x4(&mLightView);
	XMMATRIX proj = XMLoadFloat4x4(&mLightProj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	Effects::BuildShadowMapFX->SetEyePosW(mPlayer.GetCamera()->getPosition());
	Effects::BuildShadowMapFX->SetViewProj(viewProj);
	//ID3DX11EffectTechnique* alphaClippedTech = Effects::BuildShadowMapFX->BuildShadowMapAlphaClipTech;

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	ID3DX11EffectTechnique* tech = Effects::BuildShadowMapFX->TessBuildShadowMapTech;

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	//---------------------------------------------------------------
	// Draw terrain
	//---------------------------------------------------------------
	mTerrain.DrawToShadowMap(mDirect3D->GetImmediateContext(), &viewProj);

	//------------------------------------------------------------------
	// Draw opaque tessellated objects
	//------------------------------------------------------------------

	mDirect3D->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	mDirect3D->GetImmediateContext()->IASetInputLayout(InputLayouts::PosNormalTexTan);

	for (UINT pass = 0; pass < techDesc.Passes; ++pass)
	{
		for (UINT i = 0; i < mGenericInstances.size(); ++i)
		{
			world = XMLoadFloat4x4(&mGenericInstances[i].world);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BuildShadowMapFX->SetWorld(world);
			Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

			tech->GetPassByIndex(pass)->Apply(0, mDirect3D->GetImmediateContext());

			for (UINT j = 0; j < mGenericInstances[i].model->meshCount; ++j)
			{
				mGenericInstances[i].model->meshes[i].draw(mDirect3D->GetImmediateContext());
			}
		}
	}

	for (UINT pass = 0; pass < techDesc.Passes; ++pass)
	{
		for (UINT i = 0; i < mGenSkinnedInstances.size(); ++i)
		{
			world = XMLoadFloat4x4(&mGenSkinnedInstances[i].world);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BuildShadowMapFX->SetWorld(world);
			Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

			tech->GetPassByIndex(pass)->Apply(0, mDirect3D->GetImmediateContext());

			for (UINT j = 0; j < mGenSkinnedInstances[i].model->numMeshes; ++j)
			{
				mGenSkinnedInstances[i].model->meshes[i].draw(mDirect3D->GetImmediateContext());
			}
		}
	}

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	mDirect3D->GetImmediateContext()->HSSetShader(0, 0, 0);
	mDirect3D->GetImmediateContext()->DSSetShader(0, 0, 0);

	mDirect3D->GetImmediateContext()->RSSetState(0);
}

/*
void Projekt::DetectInput(float dt)
{
	/*
	DIMOUSESTATE mouseCurState;
	BYTE keyboardState[256];

	// Take over devices, to use it in this app
	DiKeyboard->Acquire();
	DiMouse->Acquire();

	// Get what states devices has
	DiMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurState);
	DiKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	//--------------------------------------------------------------------
	// Keyboard keys
	//--------------------------------------------------------------------
	if (keyboardState[DIK_ESCAPE] & 0x80)
	{
		// Menu

		SendMessage(mhMainWnd, WM_DESTROY, 0, 0);
	}

	// Up
	if (keyboardState[DIK_W] & 0x80)
	{
		mPlayer.GetCamera()->walk(30.0f*dt);
	}

	// Left
	if (keyboardState[DIK_A] & 0x80)
	{
		mPlayer.GetCamera()->strafe(-30.0f*dt);
	}

	// Down
	if (keyboardState[DIK_S] & 0x80)
	{
		mPlayer.GetCamera()->walk(-30.0f*dt);
	}

	// Right
	if (keyboardState[DIK_D] & 0x80)
	{
		mPlayer.GetCamera()->strafe(30.0f*dt);
	}

	// Mouse has moved in x-axis
	if ((mouseCurState.lX != mouseLastState.lX) || (mouseCurState.lY != mouseLastState.lY))
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(mouseCurState.lX));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(mouseCurState.lY));

		mPlayer.GetCamera()->rotateY(dx);
		mPlayer.GetCamera()->pitch(dy);
	}

	// Scroll wheel has moved
// 	if (mouseCurState.lZ != mouseLastState.lZ)
// 	{
// 	}

	// Left click
	if (mouseCurState.rgbButtons[0])
	{
	}

	// Right click
	if (mouseCurState.rgbButtons[1])
	{
	}	

	// Update mouse last state
	mouseLastState = mouseCurState;

	if (mDirectInput->GetKeyboardState()[DIK_W] && 0x80)
	{
		mPlayer.GetCamera()->walk(30.0f*dt);
	}

	// Left
	if (mDirectInput->GetKeyboardState()[DIK_A] & 0x80)
	{
		mPlayer.GetCamera()->strafe(-30.0f*dt);
	}

	// Down
	if (mDirectInput->GetKeyboardState()[DIK_S] & 0x80)
	{
		mPlayer.GetCamera()->walk(-30.0f*dt);
	}

	// Right
	if (mDirectInput->GetKeyboardState()[DIK_D] & 0x80)
	{
		mPlayer.GetCamera()->strafe(30.0f*dt);
	}

	// Mouse has moved in x-axis
	if (mDirectInput->MouseHasMoved())
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(mDirectInput->GetMouseState().lX));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(mDirectInput->GetMouseState().lY));

		mPlayer.GetCamera()->rotateY(dx);
		mPlayer.GetCamera()->pitch(dy);
	}
}

*/