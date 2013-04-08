//------------------------------------------------------------------------------------
// File: Sky.cpp
//
// This class renders a sky using a cube map
//------------------------------------------------------------------------------------

#include "Sky.h"
#include <GeometryGenerator.h>
#include "Effects.h"
#include "Vertex.h"

Sky::Sky(ID3D11Device* device, const std::wstring& cubeMapFileName, float skySphereRadius)
{
	// Create texture
	HR(D3DX11CreateShaderResourceViewFromFile(device, cubeMapFileName.c_str(), 0, 0, &mCubeMapSRV, 0));

	// Generate sphere
	GeometryGenerator::MeshData sphere;
	GeometryGenerator geoGen;
	geoGen.createSphere(skySphereRadius, 30, 30, sphere);

	std::vector<XMFLOAT3> vertices(sphere.vertices.size());

	for(size_t i = 0; i < sphere.vertices.size(); ++i)
	{
		vertices[i] = sphere.vertices[i].position;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(XMFLOAT3) * vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];

	HR(device->CreateBuffer(&vbd, &vinitData, &mVertexBuffer));


	mIndexCount = sphere.indices.size();

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(USHORT) * mIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.StructureByteStride = 0;
	ibd.MiscFlags = 0;

	std::vector<USHORT> indices16;
	indices16.assign(sphere.indices.begin(), sphere.indices.end());

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices16[0];

	HR(device->CreateBuffer(&ibd, &iinitData, &mIndexBuffer));
}


Sky::~Sky(void)
{
	ReleaseCOM(mVertexBuffer);
	ReleaseCOM(mIndexBuffer);
	ReleaseCOM(mCubeMapSRV);
}

ID3D11ShaderResourceView* Sky::cubeMapSRV()
{
	return mCubeMapSRV;
}

void Sky::draw(ID3D11DeviceContext* dc, const Camera& camera)
{
	// Center sky about eye in world space
	XMFLOAT3 eyePos = camera.getPosition();
	XMMATRIX T = XMMatrixTranslation(eyePos.x, eyePos.y, eyePos.z);

	XMMATRIX WVP = XMMatrixMultiply(T, camera.getViewProjMatrix());

	Effects::SkyFX->setWorldViewProj(WVP);
	Effects::SkyFX->setCubeMap(mCubeMapSRV);

	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	dc->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	dc->IASetInputLayout(InputLayouts::Position);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DX11_TECHNIQUE_DESC techDesc;
	Effects::SkyFX->skyTech->GetDesc( &techDesc );

	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = Effects::SkyFX->skyTech->GetPassByIndex(p);

		pass->Apply(0, dc);

		dc->DrawIndexed(mIndexCount, 0, 0);
	}
}