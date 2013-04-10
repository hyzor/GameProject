#include "RenderStates.h"

ID3D11RasterizerState* RenderStates::WireFrameRS = 0;

void RenderStates::InitAll(ID3D11Device* device)
{
	// Wireframe RS
	D3D11_RASTERIZER_DESC wireFrameDesc;
	ZeroMemory(&wireFrameDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireFrameDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireFrameDesc.CullMode = D3D11_CULL_BACK;
	wireFrameDesc.FrontCounterClockwise = false;
	wireFrameDesc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&wireFrameDesc, &WireFrameRS));

	XMVECTOR xm2;
	XMFLOAT3 xm;
}


void RenderStates::DestroyAll(void)
{
	ReleaseCOM(WireFrameRS);
}
