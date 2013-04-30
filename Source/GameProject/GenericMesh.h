//---------------------------------------------------------------------------------------
// File: MeshGeometry.h
//
// This class is used for encapsulation of vertex buffers, index buffers, and subsets.
// I.e. for organization purposes.
//---------------------------------------------------------------------------------------

#ifndef GENERICMESH_H_
#define GENERICMESH_H_

#include <d3dUtilities.h>
#include "Vertex.h"

class GenericMesh
{
public:
	GenericMesh(void);
	~GenericMesh(void);

	template <typename VertexType>
	void setVertices(ID3D11Device* device, const VertexType* vertices, UINT count);

	void setIndices(ID3D11Device* device, const UINT* indices, UINT count);
	void draw(ID3D11DeviceContext* deviceContext);

	UINT MaterialIndex;
	UINT FaceCount;
	UINT VertexCount;
	std::string Name;

	// CPU copies of mesh data 
	std::vector<Vertex::Basic32> vertices;
	std::vector<UINT> indices;

private:
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;

	DXGI_FORMAT mIndexBufferFormat; // Format either 16-bit or 32-bit

	// Cache: sizeof(VertexType)
	UINT mVertexStride;
};

template <typename VertexType>
void GenericMesh::setVertices(ID3D11Device* device, const VertexType* vertices, UINT count)
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