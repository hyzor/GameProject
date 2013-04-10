//---------------------------------------------------------------------------------------
// File: GenericMesh.h
//
// This class is used for encapsulation of vertex buffers, index buffers, and subsets.
// I.e. for organization purposes.
//---------------------------------------------------------------------------------------

#include "GenericSkinnedMesh.h"


GenericSkinnedMesh::GenericSkinnedMesh(void)
	: mVertexBuffer(0), mIndexBuffer(0),
	mIndexBufferFormat(DXGI_FORMAT_R32_UINT),
	mVertexStride(0)
{
}


GenericSkinnedMesh::~GenericSkinnedMesh(void)
{
	ReleaseCOM(mVertexBuffer);
	ReleaseCOM(mIndexBuffer);
}

void GenericSkinnedMesh::setIndices(ID3D11Device* device, const UINT* indices, UINT count)
{
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * count;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;

	HR(device->CreateBuffer(&ibd, &iinitData, &mIndexBuffer));
}

void GenericSkinnedMesh::draw(ID3D11DeviceContext* deviceContext)
{
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &mVertexStride, &offset);
	deviceContext->IASetIndexBuffer(mIndexBuffer, mIndexBufferFormat, 0);

	deviceContext->DrawIndexed(
		mIndices.size(), 
		0, 
		0);
}
