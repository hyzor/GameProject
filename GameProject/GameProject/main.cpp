#include <d3dUtilities.h>
#include <D3D11App.h>
#include <Camera.h>
#include <TextureManager.h>
#include <xnacollision.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "GenericModel.h"
#include "GenericSkinnedModel.h"
#include "Player.h"
#include "ShadowMap.h"
#include "Sky.h"
#include "FrustumCulling.h"

class Projekt : public D3D11App
{
public:
	Projekt(HINSTANCE hInstance);
	~Projekt();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

private:
	TextureManager mTextureMgr;

	// Models
	GenericModel* mGenericModel;
	GenericModel* mPlayerModel;
	std::vector<GenericModelInstance> mGenericInstances;

	// Lights
	XMFLOAT3 mOriginalLightDir[3];
	DirectionalLight mDirLights[3];

	// Shadow map
	ShadowMap* mShadowMap;

	// Scene bounding sphere
	XNA::Sphere mSceneBounds;

	// Player
	Player* mPlayer;

	// Sky
	Sky* mSky;

	// Frustum culling
	FrustumCulling* mFrustumCulling;

	// Render states
	ID3D11RasterizerState* WireFrameRS;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc(1088);

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
	: D3D11App(hInstance), mShadowMap(0)
{
	mMainWndCaption = L"DV1415 - Projekt";

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
	SafeDelete(mPlayer);
	SafeDelete(mGenericModel);
	SafeDelete(mPlayerModel);
	SafeDelete(mSky);
	SafeDelete(mShadowMap);
	SafeDelete(mFrustumCulling);
	ReleaseCOM(WireFrameRS);

	Effects::DestroyAll();
	InputLayouts::DestroyAll();
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
	
	mFrustumCulling = new FrustumCulling();

	//--------------------------------------------------------
	// Create sky
	//--------------------------------------------------------
	mSky = new Sky(mDirect3D->GetDevice(), L"Data/Textures/snowcube1024.dds", 5000.0f);

	//--------------------------------------------------------
	// Create shadow map
	//--------------------------------------------------------
	mShadowMap = new ShadowMap(mDirect3D->GetDevice(), 2048, 2048);

	//--------------------------------------------------------
	// Load models
	//--------------------------------------------------------
	mGenericModel = new GenericModel(mDirect3D->GetDevice(), mTextureMgr, "Data\\Models\\Collada\\duck.dae", L"Data\\Models\\Collada\\");

	mPlayerModel = new GenericModel(mDirect3D->GetDevice(), mTextureMgr, "Data\\Models\\OBJ\\Cop\\cop.obj", L"Data\\Models\\OBJ\\Cop\\");

	Player::InitProperties playerProp;
	playerProp.PlayerID = 0;
	playerProp.Nickname = "Hyzor";
	playerProp.Speed = 1.0f;
	playerProp.Health = 1.0f;
	playerProp.Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	playerProp.Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	playerProp.Angle = 0.0f;
	playerProp.ModelInstance.model = mPlayerModel;
	playerProp.ModelInstance.isVisible = true;

	mPlayer = new Player();
	mPlayer->Init(playerProp);
	
	//--------------------------------------------------------
	// Create model instances
	//--------------------------------------------------------
	GenericModelInstance genericInstance;
	genericInstance.model = mGenericModel;

	//--------------------------------------------------------
	// Scale, rotate and move model instances
	//--------------------------------------------------------
	XMMATRIX modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX modelRot = XMMatrixRotationY(0.0f);
	XMMATRIX modelOffset = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	//modelScale = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	modelOffset = XMMatrixTranslation(-30.0f, 15.0f, -110.0f);
	XMStoreFloat4x4(&genericInstance.world, modelScale*modelRot*modelOffset);

	//--------------------------------------------------------
	// Insert model instances to the vector
	//--------------------------------------------------------
	mGenericInstances.push_back(genericInstance);

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

	mPlayer->GetCamera()->setLens(0.25f*MathHelper::pi, AspectRatio(), 1.0f, 1000.0f);

	mPlayer->GetCamera()->computeFrustum();

	// Build the frustum from the projection matrix in view space
	//ComputeFrustumFromProjection(&mCamFrustum, &mPlayer->GetCamera()->getProjMatrix());
}

void Projekt::DrawScene()
{
	//---------------------------------------------------------------------------
	// Render scene to shadow map
	//---------------------------------------------------------------------------
	mShadowMap->BindDsvAndSetNullRenderTarget(mDirect3D->GetImmediateContext());
	mShadowMap->drawSceneToShadowMap(mGenericInstances, *mPlayer->GetCamera(), mDirect3D->GetImmediateContext());

	mDirect3D->GetImmediateContext()->RSSetState(0);

	// Restore back and depth buffer and viewport to the OM stage
	ID3D11RenderTargetView* renderTargets[1] = {mDirect3D->GetRenderTargetView()};
	mDirect3D->GetImmediateContext()->OMSetRenderTargets(1, renderTargets, mDirect3D->GetDepthStencilView());
	mDirect3D->GetImmediateContext()->ClearRenderTargetView(mDirect3D->GetRenderTargetView(), reinterpret_cast<const float*>(&Colors::LightSteelBlue));

	mDirect3D->GetImmediateContext()->ClearDepthStencilView(mDirect3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	mDirect3D->GetImmediateContext()->RSSetViewports(1, &mDirect3D->GetScreenViewport());

	//---------------------------------------------------------------------------

	// Possible Wireframe render state
	if (GetAsyncKeyState('E') & 0x8000)
		mDirect3D->GetImmediateContext()->RSSetState(WireFrameRS);

	XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowMap->getShadowTransform());

	// Camera matrices
	XMMATRIX view = mPlayer->GetCamera()->getViewMatrix();
	XMMATRIX proj = mPlayer->GetCamera()->getProjMatrix();
	XMMATRIX viewproj = mPlayer->GetCamera()->getViewProjMatrix();

	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};

	//--------------------------------------------------------------
	// Shader constants
	//--------------------------------------------------------------
	// Set per frame constants for shaders
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(mPlayer->GetCamera()->getPosition());
	Effects::BasicFX->setShadowMap(mShadowMap->getDepthMapSRV());
	Effects::BasicFX->SetCubeMap(mSky->cubeMapSRV());

// 	Effects::NormalMapFX->SetDirLights(mDirLights);
// 	Effects::NormalMapFX->SetEyePosW(mPlayer->GetCamera()->getPosition());
// 	Effects::NormalMapFX->setShadowMap(mShadowMap->getDepthMapSRV());
// 	Effects::NormalMapFX->SetCubeMap(mSky->cubeMapSRV());

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	// Draw player
	mPlayer->Draw(mDirect3D->GetImmediateContext(), mDirLights,
		mShadowMap->getDepthMapSRV(), &shadowTransform);

	// Set our effect technique to use
	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->DirLights3TexTech;
	ID3DX11EffectTechnique* activeSkinnedTech = Effects::NormalMapFX->DirLights3TexTech;

	D3DX11_TECHNIQUE_DESC techDesc;

	//--------------------------------------------------------------------------------
	// Draw opaque objects
	//--------------------------------------------------------------------------------
	mDirect3D->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mDirect3D->GetImmediateContext()->IASetInputLayout(InputLayouts::Basic32);

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

				Effects::BasicFX->SetWorld(world);
				Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
				Effects::BasicFX->SetWorldViewProj(worldViewProj);
				Effects::BasicFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
				Effects::BasicFX->setShadowTransform(world*shadowTransform);
				Effects::BasicFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

				for (UINT i = 0; i < mGenericInstances[mIndex].model->meshCount; ++i)
				{
					UINT matIndex = mGenericInstances[mIndex].model->meshes[i].MaterialIndex;

					Effects::BasicFX->SetMaterial(mGenericInstances[mIndex].model->mat[matIndex]);

					Effects::BasicFX->SetDiffuseMap(mGenericInstances[mIndex].model->diffuseMapSRV[matIndex]);
					//Effects::BasicTessFX->SetNormalMap(mGenericInstances[mIndex].model->normalMapSRV[matIndex]);

					activeTech->GetPassByIndex(p)->Apply(0, mDirect3D->GetImmediateContext());
					mGenericInstances[mIndex].model->meshes[i].draw(mDirect3D->GetImmediateContext());
				}
			}
		}
	}

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
// 	mDirect3D->GetImmediateContext()->HSSetShader(0, 0, 0);
// 	mDirect3D->GetImmediateContext()->DSSetShader(0, 0, 0);

// 	mDirect3D->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
// 	mDirect3D->GetImmediateContext()->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);

	// Skinned objects
// 	activeSkinnedTech->GetDesc(&techDesc);
// 	for (UINT p = 0; p < techDesc.Passes; ++p)
// 	{
// 		for (UINT mIndex = 0; mIndex < mGenSkinnedInstances.size(); ++mIndex)
// 		{
// 			if (mGenSkinnedInstances[mIndex].isVisible)
// 			{
// 				world = XMLoadFloat4x4(&mGenSkinnedInstances[mIndex].world);
// 				worldInvTranspose = MathHelper::InverseTranspose(world);
// 				worldViewProj = world*view*proj;
// 
// 				Effects::NormalMapFX->SetWorld(world);
// 				Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
// 				Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
// 				Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
// 				Effects::NormalMapFX->setShadowTransform(world*shadowTransform);
// 				Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
// 				Effects::NormalMapFX->setFogStart(GameSettings::Instance()->Fog()->fogStart);
// 				Effects::NormalMapFX->setFogRange(GameSettings::Instance()->Fog()->fogRange);
// 				Effects::NormalMapFX->setBoneTransforms(&mGenSkinnedInstances[mIndex].FinalTransforms[0],
// 					mGenSkinnedInstances[mIndex].FinalTransforms.size());
// 
// 
// 				Effects::BasicTessFX->setFogColor(Colors::Silver);
// 
// 				for (UINT i = 0; i < mGenSkinnedInstances[mIndex].model->numMeshes; ++i)
// 				{
// 					UINT matIndex = mGenSkinnedInstances[mIndex].model->meshes[i].mMaterialIndex;
// 
// 					Effects::NormalMapFX->SetMaterial(mGenSkinnedInstances[mIndex].model->mat[matIndex]);
// 
// 					Effects::NormalMapFX->SetDiffuseMap(mGenSkinnedInstances[mIndex].model->diffuseMapSRV[matIndex]);
// 
// 					Effects::NormalMapFX->setNormalMap(mGenSkinnedInstances[mIndex].model->normalMapSRV[matIndex]);
// 
// 					activeSkinnedTech->GetPassByIndex(p)->Apply(0, mDirect3D->GetImmediateContext());
// 					mGenSkinnedInstances[mIndex].model->meshes[i].draw(mDirect3D->GetImmediateContext());
// 				}
// 			}
// 		}
// 	}

	//---------------------------------------------------------------------------
	// Draw sky
	//---------------------------------------------------------------------------
	mSky->draw(mDirect3D->GetImmediateContext(), *mPlayer->GetCamera());

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
	if (mDirectInput->GetKeyboardState()[DIK_ESCAPE] && 0x80)
		SendMessage(mhMainWnd, WM_DESTROY, 0, 0);

	// Update objects
	mPlayer->Update(dt, mDirectInput);

	// Update shadow map
	mShadowMap->buildShadowTransform(mDirLights[0], mSceneBounds);

	// Frustum culling
	mFrustumCulling->frustumCull(mGenericInstances, *mPlayer->GetCamera());

	std::wostringstream outs;
	outs.precision(6);
	outs << L"    " << mFrustumCulling->getNumVisible() << 
		L" objects visible out of " << mGenericInstances.size();
	mMainWndCaption = outs.str();
}