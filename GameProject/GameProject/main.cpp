#include "GenericModel.h"
#include "GenericSkinnedModel.h"
#include "Player.h"
#include "ShadowMap.h"
#include "Sky.h"
#include "FrustumCulling.h"
#include "RenderStates.h"

#include "Game.h"

#include "Entity.h"

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

	std::vector<GenericModelInstance> mGenericInstances;

	// Lights
	XMFLOAT3 mOriginalLightDir[3];
	DirectionalLight mDirLights[3];

	// Shadow map
	ShadowMap* mShadowMap;

	// Scene bounding sphere
	XNA::Sphere mSceneBounds;

	// Sky
	Sky* mSky;

	// Frustum culling
	FrustumCulling* mFrustumCulling;

	Game* mGame;
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
	: D3D11App(hInstance), mShadowMap(0)
{
	mMainWndCaption = L"GameProject";

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
	SafeDelete(mSky);
	SafeDelete(mShadowMap);
	SafeDelete(mFrustumCulling);
	SafeDelete(mGame);

	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
}

bool Projekt::Init()
{
	if (!D3D11App::Init())
		return false;

	// Initialize effects, input layouts and texture manager
	Effects::InitAll(mDirect3D->GetDevice());
	InputLayouts::InitAll(mDirect3D->GetDevice());
	mTextureMgr.init(mDirect3D->GetDevice());
	RenderStates::InitAll(mDirect3D->GetDevice());

	// Create game
	mGame = new Game(mDirect3D->GetDevice(), &mTextureMgr);

	// Create sky
	mSky = new Sky(mDirect3D->GetDevice(), L"Data/Textures/snowcube1024.dds", 5000.0f);

	// Create shadow map
	mShadowMap = new ShadowMap(mDirect3D->GetDevice(), 2048, 2048);

	// Create frustum culling
	mFrustumCulling = new FrustumCulling();

	//--------------------------------------------------------
	// Compute scene bounding box
	//--------------------------------------------------------
// 	XMFLOAT3 minPt(+MathHelper::infinity, +MathHelper::infinity, +MathHelper::infinity);
// 	XMFLOAT3 maxPt(-MathHelper::infinity, -MathHelper::infinity, -MathHelper::infinity);
// 
// 	// Get vertex positions from all models
// 	for (UINT i = 0; i < mGenericInstances.size(); ++i)
// 	{
// 		for (UINT j = 0; j < mGenericInstances[i].model->vertices.size(); ++j)
// 		{
// 			XMFLOAT3 vPos = mGenericInstances[i].model->vertices[j]->position;
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
// 
// 	// Sphere center is at half of these new dimensions
// 	mSceneBounds.Center = XMFLOAT3(	0.5f*(minPt.x + maxPt.x),
// 		0.5f*(minPt.y + maxPt.y),
// 		0.5f*(minPt.z + maxPt.z));
// 
// 	// Calculate the sphere radius
// 	XMFLOAT3 extent(0.5f*(maxPt.x - minPt.x),
// 		0.5f*(maxPt.y - minPt.y),
// 		0.5f*(maxPt.z - minPt.z));
// 
// 	mSceneBounds.Radius = sqrtf(extent.x*extent.x + extent.y*extent.y + extent.z*extent.z);

	OnResize();

	return true;
}

void Projekt::OnResize()
{
	D3D11App::OnResize();

	mGame->GetCamera()->SetLens(0.25f*MathHelper::pi, AspectRatio(), 1.0f, 1000.0f);
	mGame->GetCamera()->ComputeFrustum();

	XMVECTOR myVec;
	myVec.m128_f32[0];
}

void Projekt::DrawScene()
{
	//---------------------------------------------------------------------------
	// Render scene to shadow map
	//---------------------------------------------------------------------------
	mShadowMap->BindDsvAndSetNullRenderTarget(mDirect3D->GetImmediateContext());
	mShadowMap->DrawSceneToShadowMap(mGenericInstances, *mGame->GetCamera(), mDirect3D->GetImmediateContext());

	mDirect3D->GetImmediateContext()->RSSetState(0);

	// Restore back and depth buffer and viewport to the OM stage
	ID3D11RenderTargetView* renderTargets[1] = {mDirect3D->GetRenderTargetView()};
	mDirect3D->GetImmediateContext()->OMSetRenderTargets(1, renderTargets, mDirect3D->GetDepthStencilView());
	mDirect3D->GetImmediateContext()->ClearRenderTargetView(mDirect3D->GetRenderTargetView(), reinterpret_cast<const float*>(&Colors::LightSteelBlue));

	mDirect3D->GetImmediateContext()->ClearDepthStencilView(mDirect3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	mDirect3D->GetImmediateContext()->RSSetViewports(1, &mDirect3D->GetScreenViewport());

	//---------------------------------------------------------------------------

	// Possible Wireframe render state
	if (mDirectInput->GetKeyboardState()[DIK_E] && 0x80)
		mDirect3D->GetImmediateContext()->RSSetState(RenderStates::WireFrameRS);

	//--------------------------------------------------------------
	// Set shader constants
	//--------------------------------------------------------------
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(mGame->GetCamera()->GetPosition());
	Effects::BasicFX->SetShadowMap(mShadowMap->getDepthMapSRV());
	Effects::BasicFX->SetCubeMap(mSky->cubeMapSRV());

	Effects::NormalMapFX->SetDirLights(mDirLights);
	Effects::NormalMapFX->SetEyePosW(mGame->GetCamera()->GetPosition());
	Effects::NormalMapFX->SetShadowMap(mShadowMap->getDepthMapSRV());
	Effects::NormalMapFX->SetCubeMap(mSky->cubeMapSRV());

	// Draw game
	mGame->Draw(mDirect3D->GetImmediateContext(), mShadowMap);

	// Draw sky
	mSky->draw(mDirect3D->GetImmediateContext(), *mGame->GetCamera());

	// Unbind shadow map and AmbientMap as a shader input because we are going to render
	// to it next frame.  These textures can be at any slot, so clear all slots.
	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	mDirect3D->GetImmediateContext()->PSSetShaderResources(0, 16, nullSRV);

	// Restore default states
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
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
	mGame->Update(dt, mDirectInput);

	// Update shadow map
	mShadowMap->BuildShadowTransform(mDirLights[0], mSceneBounds);

	// Frustum culling
	mFrustumCulling->FrustumCull(mGenericInstances, *mGame->GetCamera());

	std::wostringstream outs;
	outs.precision(6);
	outs << L"    " << mFrustumCulling->GetNumVisible() << 
		L" objects visible out of " << mGenericInstances.size();
	mMainWndCaption = outs.str();
}