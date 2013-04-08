#ifndef TERRAIN_H
#define TERRAIN_H

#include <windows.h>
#include <xnamath.h>
#include <d3d11.h>
#include "LightDef.h"
#include <vector>
#include "D3D11App.h"
#include <fstream>
#include "Vertex.h"
#include "d3dx11effect.h"
#include "Effects.h"
#include <algorithm>
#include <Camera.h>

#include "GameSettings.h"

class Terrain
{
public:
	Terrain(void);
	~Terrain(void);

	float GetWidth() const;
	float GetDepth() const;
	float GetHeight(float x, float z) const;

	XMMATRIX GetWorld() const;
	void SetWorld(CXMMATRIX M);

	// Terrain properties
	struct InitInfo
	{
		// RAW heightmap filename
		std::wstring HeightMapFilename;

		// Texture filenames
		std::wstring LayerMapFilename0;
		std::wstring LayerMapFilename1;
		std::wstring LayerMapFilename2;
		std::wstring LayerMapFilename3;
		std::wstring LayerMapFilename4;
		std::wstring BlendMapFilename;

		float HeightScale;	// Height scale to apply after loading heightmap
		UINT HeightmapWidth;
		UINT HeightmapHeight;
		float CellSpacing;	// Cell spacing along x and z-axes
	};

	void Init(ID3D11Device* device, ID3D11DeviceContext* dc, const InitInfo& initInfo);

	void Draw(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);

	void DrawShadowed(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3], 
		ID3D11ShaderResourceView* shadowMap, XMMATRIX* shadowTransform);

	void DrawToShadowMap(ID3D11DeviceContext* dc, XMMATRIX* lightViewProj);

	std::vector<Vertex::TerrainNormal> getPatchVertices() const;

private:
	void LoadHeightmap();

	void BuildQuadPatchVB(ID3D11Device* device);
	void BuildQuadPatchIB(ID3D11Device* device);

	// Build a shader resource view for tessellation and displacement mapping (Creates 2D texture)
	void BuildHeightmapSRV(ID3D11Device* device);

	// Functions used for terrain smoothing
	bool InBounds(int i, int j); // Used to determine if an index has 8 neighboring indices
	float Average(int i, int j); // Calculates average value for index by looking at 8 neighbors
	void Smooth();

	// Order: left, right, bottom, top, near, far.
	void ExtractFrustumPlanes(XMFLOAT4 planes[6], CXMMATRIX M);

	// Number of cells a patch contains
	// Tessellation to 64 is the maximum
	static const int CellsPerPatch = 64;
	
	ID3D11Buffer* mQuadPatchVB; // Quad patch vertex buffer
	ID3D11Buffer* mQuadPatchIB; // Quad patch index buffer

	ID3D11ShaderResourceView* mLayerMapArraySRV;
	ID3D11ShaderResourceView* mBlendMapSRV;
	ID3D11ShaderResourceView* mHeightMapSRV;

	UINT mNumPatchVertices;
	UINT mNumPatchQuadFaces;

	UINT mNumPatchVertRows;
	UINT mNumPatchVertCols;

	InitInfo mInfo; // Terrain properties object

	XMFLOAT4X4 mWorld;

	Material mMat;

	std::vector<XMFLOAT2> mPatchBoundsY;
	std::vector<float> mHeightmap;

	// CPU copy of quad patch vertices
	std::vector<Vertex::TerrainNormal> mPatchVertices;
};

#endif