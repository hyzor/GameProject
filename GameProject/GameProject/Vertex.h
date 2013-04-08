#ifndef VERTEX_H
#define VERTEX_H

#include <d3dUtilities.h>
#include "Effects.h"

// Vertices
namespace Vertex
{
	struct Basic32
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texCoord;
	};

	struct Terrain
	{
		XMFLOAT3 position;
		XMFLOAT2 texCoord;
		XMFLOAT2 boundsY;
	};

	struct TerrainNormal
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texCoord;
		XMFLOAT2 boundsY;
	};

	struct Particle
	{
		XMFLOAT3 InitialPos;
		XMFLOAT3 InitialVel;
		XMFLOAT2 Size;
		float Age;
		unsigned int Type;
	};

	struct PosNormalTexTan
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texCoord;
		XMFLOAT4 tangentU;
	};

	struct PosNormalTexTanSkinned
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texCoord;
		XMFLOAT4 tangentU;
		XMFLOAT3 weights;
		BYTE boneIndices[4];
	};
}

// Input layout descriptions
class InputLayoutDesc
{
public:
	static const D3D11_INPUT_ELEMENT_DESC Position[1];
	static const D3D11_INPUT_ELEMENT_DESC Basic32[3];
	static const D3D11_INPUT_ELEMENT_DESC Terrain[3];
	static const D3D11_INPUT_ELEMENT_DESC TerrainNormal[4];
	static const D3D11_INPUT_ELEMENT_DESC Particle[5];
	static const D3D11_INPUT_ELEMENT_DESC PosNormalTexTan[4];
	static const D3D11_INPUT_ELEMENT_DESC PosNormalTexTanSkinned[6];
};

// Input layouts
class InputLayouts
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11InputLayout* Position;
	static ID3D11InputLayout* Basic32;
	static ID3D11InputLayout* Terrain;
	static ID3D11InputLayout* TerrainNormal;
	static ID3D11InputLayout* Particle;
	static ID3D11InputLayout* PosNormalTexTan;
	static ID3D11InputLayout* PosNormalTexTanSkinned;
};

#endif