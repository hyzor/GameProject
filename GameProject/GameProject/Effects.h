//------------------------------------------------------------------------------------
// File: Effects.h
//
// Encapsulates all the effects (.fx files)
//------------------------------------------------------------------------------------

#ifndef EFFECTS_H
#define EFFECTS_H

#include <d3dUtilities.h>
#include <LightDef.h>

class Effect
{
public:
	Effect(ID3D11Device* device, const std::wstring& filename);
	virtual ~Effect();

private:
	Effect(const Effect& rhs);
	Effect& operator=(const Effect& rhs);

protected:
	ID3DX11Effect* mFX;
};

//====================================================================
// Basic effect
//====================================================================
class BasicEffect : public Effect
{
public:
	BasicEffect(ID3D11Device* device, const std::wstring& filename);
	~BasicEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProjTex(CXMMATRIX M)               { WorldViewProjTex->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }

	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }

	void SetDirLights(const DirectionalLight* lights)   { dirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void setPointLight(const PointLight* light)			{ pointLight->SetRawValue(light, 0, sizeof(PointLight)); }

	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }

	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetCubeMap(ID3D11ShaderResourceView* tex) { CubeMap->SetResource(tex); }

	void setShadowMap(ID3D11ShaderResourceView* tex) { shadowMap->SetResource(tex); }
	void setShadowTransform(CXMMATRIX M) { shadowTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }

	void setFogRange(float f) { fogRange->SetFloat(f); }
	void setFogStart(float f) { fogStart->SetFloat(f); }
	void setFogColor(const FXMVECTOR v) { fogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }

	//-------------------------------------------------------
	// Techniques
	//-------------------------------------------------------
	ID3DX11EffectTechnique* PointLight1Tech;
	ID3DX11EffectTechnique* DirLights3Tech;

	ID3DX11EffectTechnique* PointLight1TexTech;
	ID3DX11EffectTechnique* DirLights3TexTech;

	ID3DX11EffectTechnique* DirLights3FogTech;

	ID3DX11EffectTechnique* DirLights3FogTexTech;

	ID3DX11EffectTechnique* DirLights3FogReflectionTech;

	// Matrices
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProjTex;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;

	// Eye position
	ID3DX11EffectVectorVariable* EyePosW;

	// Material
	ID3DX11EffectVariable* Mat;

	// Lights
	ID3DX11EffectVariable* pointLight;
	ID3DX11EffectVariable* dirLights;

	// Shadow mapping
	ID3DX11EffectShaderResourceVariable* shadowMap;
	ID3DX11EffectMatrixVariable* shadowTransform;

	// Texture
	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* CubeMap;

	// Fogging
	ID3DX11EffectScalarVariable* fogRange;
	ID3DX11EffectScalarVariable* fogStart;
	ID3DX11EffectVectorVariable* fogColor;
};

//====================================================================
// Basic tessellation effect
//====================================================================
class BasicTessEffect : public Effect
{
public:
	BasicTessEffect(ID3D11Device* device, const std::wstring& filename);
	~BasicTessEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProjTex(CXMMATRIX M)               { WorldViewProjTex->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }

	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }

	void SetDirLights(const DirectionalLight* lights)   { dirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void setPointLight(const PointLight* light)			{ pointLight->SetRawValue(light, 0, sizeof(PointLight)); }

	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }

	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetNormalMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetCubeMap(ID3D11ShaderResourceView* tex) { CubeMap->SetResource(tex); }

	void setShadowMap(ID3D11ShaderResourceView* tex) { shadowMap->SetResource(tex); }
	void setShadowTransform(CXMMATRIX M) { shadowTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }

	void setFogRange(float f) { fogRange->SetFloat(f); }
	void setFogStart(float f) { fogStart->SetFloat(f); }
	void setFogColor(const FXMVECTOR v) { fogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }

	void SetMinTessDistance(float f)                       { MinDist->SetFloat(f); }
	void SetMaxTessDistance(float f)                       { MaxDist->SetFloat(f); }
	void SetMinTessFactor(float f)                       { MinTess->SetFloat(f); }
	void SetMaxTessFactor(float f)                       { MaxTess->SetFloat(f); }

	//-------------------------------------------------------
	// Techniques
	//-------------------------------------------------------
	ID3DX11EffectTechnique* TessDirLights3FogTexTech;
	ID3DX11EffectTechnique* TessDirLights3Tech;

	// Matrices
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProjTex;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;

	// Eye position
	ID3DX11EffectVectorVariable* EyePosW;

	// Material
	ID3DX11EffectVariable* Mat;

	// Lights
	ID3DX11EffectVariable* pointLight;
	ID3DX11EffectVariable* dirLights;

	// Shadow mapping
	ID3DX11EffectShaderResourceVariable* shadowMap;
	ID3DX11EffectMatrixVariable* shadowTransform;

	// Textures
	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;
	ID3DX11EffectShaderResourceVariable* CubeMap;

	// Fogging
	ID3DX11EffectScalarVariable* fogRange;
	ID3DX11EffectScalarVariable* fogStart;
	ID3DX11EffectVectorVariable* fogColor;

	// Tessellation
	ID3DX11EffectScalarVariable* MinDist;
	ID3DX11EffectScalarVariable* MaxDist;
	ID3DX11EffectScalarVariable* MinTess;
	ID3DX11EffectScalarVariable* MaxTess;
};

//====================================================================
// Shadow map effect
//====================================================================
class BuildShadowMapEffect : public Effect
{
public:
	BuildShadowMapEffect(ID3D11Device* device, const std::wstring& fileName);
	~BuildShadowMapEffect();

	void SetViewProj(CXMMATRIX M)                       { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }

	void SetHeightScale(float f)                        { HeightScale->SetFloat(f); }
	void SetMaxTessDistance(float f)                    { MaxTessDistance->SetFloat(f); }
	void SetMinTessDistance(float f)                    { MinTessDistance->SetFloat(f); }
	void SetMinTessFactor(float f)                      { MinTessFactor->SetFloat(f); }
	void SetMaxTessFactor(float f)                      { MaxTessFactor->SetFloat(f); }

	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetNormalMap(ID3D11ShaderResourceView* tex) { NormalMap->SetResource(tex); }

	ID3DX11EffectTechnique* BuildShadowMapTech;
	ID3DX11EffectTechnique* BuildShadowMapAlphaClipTech;
	ID3DX11EffectTechnique* TessBuildShadowMapTech;
	ID3DX11EffectTechnique* TessBuildShadowMapAlphaClipTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectScalarVariable* HeightScale;
	ID3DX11EffectScalarVariable* MaxTessDistance;
	ID3DX11EffectScalarVariable* MinTessDistance;
	ID3DX11EffectScalarVariable* MinTessFactor;
	ID3DX11EffectScalarVariable* MaxTessFactor;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;
};

//==============================================================================
// Terrain effect
//==============================================================================
class TerrainEffect : public Effect
{
public:
	TerrainEffect(ID3D11Device* device, const std::wstring& filename);
	~TerrainEffect();

	void SetViewProj(CXMMATRIX M) { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }

	void SetDirLights(const DirectionalLight* lights) { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat) { Mat->SetRawValue(&mat, 0, sizeof(Material)); }

	// Set SRVs
	void SetLayerMapArray(ID3D11ShaderResourceView* tex)   { LayerMapArray->SetResource(tex); }
	void SetBlendMap(ID3D11ShaderResourceView* tex)        { BlendMap->SetResource(tex); }
	void SetHeightMap(ID3D11ShaderResourceView* tex)       { HeightMap->SetResource(tex); }

	void SetMinTessDistance(float f)                       { MinDist->SetFloat(f); }
	void SetMaxTessDistance(float f)                       { MaxDist->SetFloat(f); }
	void SetMinTessFactor(float f)                       { MinTess->SetFloat(f); }
	void SetMaxTessFactor(float f)                       { MaxTess->SetFloat(f); }
	void SetTexelCellSpaceU(float f)               { TexelCellSpaceU->SetFloat(f); }
	void SetTexelCellSpaceV(float f)               { TexelCellSpaceV->SetFloat(f); }
	void SetWorldCellSpace(float f)                { WorldCellSpace->SetFloat(f); }
	void SetWorldFrustumPlanes(XMFLOAT4 planes[6]) { WorldFrustumPlanes->SetFloatVectorArray(reinterpret_cast<float*>(planes), 0, 6); }

	void setFogRange(float f) { fogRange->SetFloat(f); }
	void setFogStart(float f) { fogStart->SetFloat(f); }
	void setFogColor(const FXMVECTOR v) { fogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }

	void setShadowMap(ID3D11ShaderResourceView* tex) { shadowMap->SetResource(tex); }
	void setShadowTransform(CXMMATRIX M) { shadowTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;

	ID3DX11EffectShaderResourceVariable* LayerMapArray;
	ID3DX11EffectShaderResourceVariable* BlendMap;
	ID3DX11EffectShaderResourceVariable* HeightMap;

	ID3DX11EffectTechnique* Tech1;
	ID3DX11EffectTechnique* DirLights3Tech;
	ID3DX11EffectTechnique* DirLights3FogTech;

	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;

	ID3DX11EffectScalarVariable* MinDist;
	ID3DX11EffectScalarVariable* MaxDist;
	ID3DX11EffectScalarVariable* MinTess;
	ID3DX11EffectScalarVariable* MaxTess;
	ID3DX11EffectScalarVariable* TexelCellSpaceU;
	ID3DX11EffectScalarVariable* TexelCellSpaceV;
	ID3DX11EffectScalarVariable* WorldCellSpace;
	ID3DX11EffectVectorVariable* WorldFrustumPlanes;

	// Fogging
	ID3DX11EffectScalarVariable* fogRange;
	ID3DX11EffectScalarVariable* fogStart;
	ID3DX11EffectVectorVariable* fogColor;

	// Shadow mapping
	ID3DX11EffectShaderResourceVariable* shadowMap;
	ID3DX11EffectMatrixVariable* shadowTransform;
};

//====================================================================
// Particle effect
//====================================================================
class ParticleEffect : public Effect
{
public:
	ParticleEffect(ID3D11Device* device, const std::wstring& filename);
	~ParticleEffect();

	void SetViewProj(CXMMATRIX M) { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }

	void SetGameTime(float f) { GameTime->SetFloat(f); }
	void SetTimeStep(float f) { TimeStep->SetFloat(f); }

	void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetEmitPosW(const XMFLOAT3& v) { EmitPosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetEmitDirW(const XMFLOAT3& v) { EmitDirW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }

	void SetTexArray(ID3D11ShaderResourceView* tex) { TexArray->SetResource(tex); }
	void SetRandomTex(ID3D11ShaderResourceView* tex) { RandomTex->SetResource(tex); }

	ID3DX11EffectTechnique* StreamOutTech;
	ID3DX11EffectTechnique* DrawTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* EmitPosW;
	ID3DX11EffectVectorVariable* EmitDirW;

	ID3DX11EffectShaderResourceVariable* TexArray;
	ID3DX11EffectShaderResourceVariable* RandomTex;

	ID3DX11EffectScalarVariable* GameTime;
	ID3DX11EffectScalarVariable* TimeStep;
};

//==============================================================================
// Sky effect
//==============================================================================
class SkyEffect : public Effect
{
public:
	SkyEffect(ID3D11Device* device, const std::wstring& filename);
	~SkyEffect();

	void setWorldViewProj(CXMMATRIX m) { worldViewProj->SetMatrix(reinterpret_cast<const float*>(&m)); }
	void setCubeMap(ID3D11ShaderResourceView* _cubeMap) { cubeMap->SetResource(_cubeMap); }

	ID3DX11EffectTechnique* skyTech;

	ID3DX11EffectMatrixVariable* worldViewProj;

	ID3DX11EffectShaderResourceVariable* cubeMap;
};

//==============================================================================
// Normal map effect
//==============================================================================
class NormalMapEffect : public Effect
{
public:
	NormalMapEffect(ID3D11Device* device, const std::wstring& filename);
	~NormalMapEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProjTex(CXMMATRIX M)               { WorldViewProjTex->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }

	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }

	void SetDirLights(const DirectionalLight* lights)   { dirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }

	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }

	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetCubeMap(ID3D11ShaderResourceView* tex) { CubeMap->SetResource(tex); }
	void setNormalMap(ID3D11ShaderResourceView* tex) { NormalMap->SetResource(tex); }

	// Shadow map
	void setShadowMap(ID3D11ShaderResourceView* tex) { shadowMap->SetResource(tex); }
	void setShadowTransform(CXMMATRIX M) { shadowTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }

	// Fog
	void setFogRange(float f) { fogRange->SetFloat(f); }
	void setFogStart(float f) { fogStart->SetFloat(f); }
	void setFogColor(const FXMVECTOR v) { fogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }

	void setBoneTransforms(const XMFLOAT4X4* M, int cnt) { BoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt); }

	//-------------------------------------------------------
	// Techniques
	//-------------------------------------------------------
	ID3DX11EffectTechnique* DirLights3TexTech;

	// Matrices
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProjTex;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;

	ID3DX11EffectMatrixVariable* BoneTransforms;

	// Eye position
	ID3DX11EffectVectorVariable* EyePosW;

	// Material
	ID3DX11EffectVariable* Mat;

	// Lights
	ID3DX11EffectVariable* dirLights;

	// Shadow mapping
	ID3DX11EffectShaderResourceVariable* shadowMap;
	ID3DX11EffectMatrixVariable* shadowTransform;

	// Texture
	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* CubeMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;

	// Fogging
	ID3DX11EffectScalarVariable* fogRange;
	ID3DX11EffectScalarVariable* fogStart;
	ID3DX11EffectVectorVariable* fogColor;

};

//==============================================================================
// Effect instances
//==============================================================================
class Effects
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static BasicEffect* BasicFX;
	static BuildShadowMapEffect* BuildShadowMapFX;
	static TerrainEffect* TerrainFX;
	static ParticleEffect* FireFX;
	static SkyEffect* SkyFX;
	static BasicTessEffect* BasicTessFX;
	static NormalMapEffect* NormalMapFX;
};

#endif