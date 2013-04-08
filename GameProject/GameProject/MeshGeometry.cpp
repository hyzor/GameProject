//---------------------------------------------------------------------------------------
// File: MeshGeometry.h
//
// This class is used for encapsulation of vertex buffers, index buffers, and subsets.
// I.e. for organization purposes.
//---------------------------------------------------------------------------------------

#include "MeshGeometry.h"


MeshGeometry::MeshGeometry(void)
	: mVertexBuffer(0), mIndexBuffer(0),
	mIndexBufferFormat(DXGI_FORMAT_R32_UINT),
	mVertexStride(0)
{
}


MeshGeometry::~MeshGeometry(void)
{
	ReleaseCOM(mVertexBuffer);
	ReleaseCOM(mIndexBuffer);
}

void MeshGeometry::setIndices(ID3D11Device* device, const UINT* indices, UINT count)
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

void MeshGeometry::setSubsetTable(std::vector<Subset>& subsetTable)
{
	mSubsetTable = subsetTable;
}

void MeshGeometry::draw(ID3D11DeviceContext* deviceContext, UINT subsetId)
{
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &mVertexStride, &offset);
	deviceContext->IASetIndexBuffer(mIndexBuffer, mIndexBufferFormat, 0);

	deviceContext->DrawIndexed(
		mSubsetTable[subsetId].FaceCount*3, 
		mSubsetTable[subsetId].FaceStart*3, 
		0);
}
