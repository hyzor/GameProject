//---------------------------------------------------------------------------------------
// File: MeshGeometry.h
//
// This class is used for encapsulation of vertex buffers, index buffers, and subsets.
// I.e. for organization purposes.
//---------------------------------------------------------------------------------------

#ifndef MESHGEOMETRY_H_
#define MESHGEOMETRY_H_

#include <d3dUtilities.h>

class MeshGeometry
{
public:
	struct Subset
	{
		Subset() :
			Id(-1), VertexStart(0), VertexCount(0),
				FaceStart(0), FaceCount(0), MaterialIndex(0), Name("")
			{}

			UINT Id;
			UINT VertexStart;
			UINT VertexCount;
			UINT FaceStart;
			UINT FaceCount;

			UINT MaterialIndex;
			std::string Name;
	};

public:
	MeshGeometry(void);
	~MeshGeometry(void);

	template <typename VertexType>
	void setVertices(ID3D11Device* device, const VertexType* vertices, UINT count);

	void setIndices(ID3D11Device* device, const UINT* indices, UINT count);

	void setSubsetTable(std::vector<Subset>& subsetTable);
	void draw(ID3D11DeviceContext* deviceContext, UINT subsetId);

private:
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;

	DXGI_FORMAT mIndexBufferFormat; // Format either 16-bit or 32-bit

	// Cache: sizeof(VertexType)
	UINT mVertexStride;

	std::vector<Subset> mSubsetTable;
};

template <typename VertexType>
void MeshGeometry::setVertices(ID3D11Device* device, const VertexType* vertices, UINT count)
{
	ReleaseCOM(mVertexBuffer);

	mVertexStride = sizeof(VertexType);

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexType) * count;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vInitData;
	vInitData.pSysMem = vertices;

	HR(device->CreateBuffer(&vbd, &vInitData, &mVertexBuffer));
}

#endif