//------------------------------------------------------------------------------------
// File: Effects.cp
//
// Encapsulates all the effects (.fx files)
//------------------------------------------------------------------------------------

#include "Effects.h"

//==============================================================================
// Abstract effect
//==============================================================================
Effect::Effect(ID3D11Device* device, const std::wstring& filename)
{
	mFX = 0;
	std::ifstream fin(filename, std::ios::binary);
	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size,
		0, device, &mFX));
}

Effect::~Effect()
{
	ReleaseCOM(mFX);
}

//==============================================================================
// Basic effect
//==============================================================================
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	//---------------------------------------------------------------
	// Techniques
	//---------------------------------------------------------------
	PointLight1Tech = mFX->GetTechniqueByName("PointLight1");
	DirLights3Tech = mFX->GetTechniqueByName("DirLights3");

	PointLight1TexTech = mFX->GetTechniqueByName("PointLight1Tex");
	DirLights3TexTech = mFX->GetTechniqueByName("DirLights3Tex");

	DirLights3TexAlphaClipTech = mFX->GetTechniqueByName("DirLights3TexAlphaClip");

	DirLights3FogTech = mFX->GetTechniqueByName("DirLights3Fog");

	DirLights3FogTexTech = mFX->GetTechniqueByName("DirLights3FogTex");

	DirLights3FogReflectionTech = mFX->GetTechniqueByName("DirLights3FogReflection");

	//-------------

	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	WorldViewProjTex  = mFX->GetVariableByName("gWorldViewProjTex")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();

	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();

	PointLight = mFX->GetVariableByName("gPointLight");
	DirLights = mFX->GetVariableByName("gDirLights");

	ShadowTransform = mFX->GetVariableByName("gShadowTransform")->AsMatrix();
	ShadowMap = mFX->GetVariableByName("gShadowMap")->AsShaderResource();

	Mat               = mFX->GetVariableByName("gMaterial");
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	CubeMap = mFX->GetVariableByName("gCubeMap")->AsShaderResource();

	FogStart = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange = mFX->GetVariableByName("gFogRange")->AsScalar();
	FogColor = mFX->GetVariableByName("gFogColor")->AsVector();
}

BasicEffect::~BasicEffect()
{

}

//==============================================================================
// Basic tessellation effect
//==============================================================================
BasicTessEffect::BasicTessEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	//---------------------------------------------------------------
	// Techniques
	//---------------------------------------------------------------
	TessDirLights3FogTexTech = mFX->GetTechniqueByName("TessDirLights3FogTex");
	TessDirLights3Tech = mFX->GetTechniqueByName("TessDirLights3");

	//-------------

	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	WorldViewProjTex  = mFX->GetVariableByName("gWorldViewProjTex")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();

	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();

	pointLight = mFX->GetVariableByName("gPointLight");
	dirLights = mFX->GetVariableByName("gDirLights");

	shadowTransform = mFX->GetVariableByName("gShadowTransform")->AsMatrix();
	shadowMap = mFX->GetVariableByName("gShadowMap")->AsShaderResource();

	Mat               = mFX->GetVariableByName("gMaterial");
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	NormalMap = mFX->GetVariableByName("gNormalMap")->AsShaderResource();
	CubeMap = mFX->GetVariableByName("gCubeMap")->AsShaderResource();

	fogStart = mFX->GetVariableByName("gFogStart")->AsScalar();
	fogRange = mFX->GetVariableByName("gFogRange")->AsScalar();
	fogColor = mFX->GetVariableByName("gFogColor")->AsVector();

	MinDist = mFX->GetVariableByName("gMinDist")->AsScalar();
	MaxDist = mFX->GetVariableByName("gMaxDist")->AsScalar();
	MinTess = mFX->GetVariableByName("gMinTess")->AsScalar();
	MaxTess = mFX->GetVariableByName("gMaxTess")->AsScalar();
}

BasicTessEffect::~BasicTessEffect()
{

}

//==============================================================================
// Shadow map effect
//==============================================================================
BuildShadowMapEffect::BuildShadowMapEffect(ID3D11Device* device, const std::wstring& fileName)
	: Effect(device, fileName)
{
	BuildShadowMapTech           = mFX->GetTechniqueByName("BuildShadowMapTech");
	BuildShadowMapAlphaClipTech  = mFX->GetTechniqueByName("BuildShadowMapAlphaClipTech");

	TessBuildShadowMapTech           = mFX->GetTechniqueByName("TessBuildShadowMapTech");
	TessBuildShadowMapAlphaClipTech  = mFX->GetTechniqueByName("TessBuildShadowMapAlphaClipTech");

	ViewProj          = mFX->GetVariableByName("gViewProj")->AsMatrix();
	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
	HeightScale       = mFX->GetVariableByName("gHeightScale")->AsScalar();
	MaxTessDistance   = mFX->GetVariableByName("gMaxTessDistance")->AsScalar();
	MinTessDistance   = mFX->GetVariableByName("gMinTessDistance")->AsScalar();
	MinTessFactor     = mFX->GetVariableByName("gMinTessFactor")->AsScalar();
	MaxTessFactor     = mFX->GetVariableByName("gMaxTessFactor")->AsScalar();
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	NormalMap         = mFX->GetVariableByName("gNormalMap")->AsShaderResource();
}

BuildShadowMapEffect::~BuildShadowMapEffect()
{

}

//==============================================================================
// Particle effect
//==============================================================================
ParticleEffect::ParticleEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	StreamOutTech = mFX->GetTechniqueByName("StreamOutTech");
	DrawTech = mFX->GetTechniqueByName("DrawTech");

	ViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	EmitPosW = mFX->GetVariableByName("gEmitPosW")->AsVector();
	EmitDirW = mFX->GetVariableByName("gEmitDirW")->AsVector();
	TexArray = mFX->GetVariableByName("gTexArray")->AsShaderResource();
	RandomTex = mFX->GetVariableByName("gRandomTex")->AsShaderResource();

	GameTime = mFX->GetVariableByName("gGameTime")->AsScalar();
	TimeStep = mFX->GetVariableByName("gTimeStep")->AsScalar();
}

ParticleEffect::~ParticleEffect()
{

}

//==============================================================================
// Sky effect
//==============================================================================
SkyEffect::SkyEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	skyTech = mFX->GetTechniqueByName("SkyTech");

	worldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	cubeMap = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
	inMenu = mFX->GetVariableByName("inMenu")->AsScalar();
}

SkyEffect::~SkyEffect()
{

};

//==============================================================================
// Normal effect
//==============================================================================
NormalMapEffect::NormalMapEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	//---------------------------------------------------------------
	// Techniques
	//---------------------------------------------------------------
	DirLights3TexTech = mFX->GetTechniqueByName("DirLights3Tex");
	DirLights3TexAlphaClipTech = mFX->GetTechniqueByName("DirLights3TexAlphaClip");
	DirLights3TexSkinnedTech = mFX->GetTechniqueByName("DirLights3TexSkinned");

	//-------------

	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	WorldViewProjTex  = mFX->GetVariableByName("gWorldViewProjTex")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();

	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();

	dirLights = mFX->GetVariableByName("gDirLights");

	shadowTransform = mFX->GetVariableByName("gShadowTransform")->AsMatrix();
	shadowMap = mFX->GetVariableByName("gShadowMap")->AsShaderResource();

	Mat               = mFX->GetVariableByName("gMaterial");
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	CubeMap = mFX->GetVariableByName("gCubeMap")->AsShaderResource();

	NormalMap = mFX->GetVariableByName("gNormalMap")->AsShaderResource();

	fogStart = mFX->GetVariableByName("gFogStart")->AsScalar();
	fogRange = mFX->GetVariableByName("gFogRange")->AsScalar();
	fogColor = mFX->GetVariableByName("gFogColor")->AsVector();

	BoneTransforms = mFX->GetVariableByName("gBoneTransforms")->AsMatrix();
}

NormalMapEffect::~NormalMapEffect()
{

}

//==============================================================================
// Effect instances
//==============================================================================
BasicEffect* Effects::BasicFX = 0;
BuildShadowMapEffect* Effects::BuildShadowMapFX = 0;
SkyEffect* Effects::SkyFX = 0;
BasicTessEffect* Effects::BasicTessFX = 0;
NormalMapEffect* Effects::NormalMapFX = 0;

void Effects::InitAll(ID3D11Device* device)
{
	BasicFX = new BasicEffect(device, L"Data/FX/Basic.fxo");
	BuildShadowMapFX = new BuildShadowMapEffect(device, L"Data/FX/BuildShadowMap.fxo");
	SkyFX = new SkyEffect(device, L"Data/FX/Sky.fxo");
	BasicTessFX = new BasicTessEffect(device, L"Data/FX/BasicTess.fxo");
	NormalMapFX = new NormalMapEffect(device, L"Data/FX/NormalMap.fxo");
}

void Effects::DestroyAll()
{
	SafeDelete(BasicFX);
	SafeDelete(BuildShadowMapFX);
	SafeDelete(SkyFX);
	SafeDelete(BasicTessFX);
	SafeDelete(NormalMapFX);
}