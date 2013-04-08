#include "Terrain.h"


Terrain::Terrain(void)
{
	XMStoreFloat4x4(&mWorld, XMMatrixIdentity());

	mMat.Ambient  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMat.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMat.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 64.0f);
	mMat.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}


Terrain::~Terrain(void)
{
	ReleaseCOM(mQuadPatchVB);
	ReleaseCOM(mQuadPatchIB);
	
// 	ReleaseCOM(mLayerMapArraySRV);
// 	ReleaseCOM(mBlendMapSRV);
// 	ReleaseCOM(mHeightMapSRV);
}

float Terrain::GetWidth() const
{
	return (mInfo.HeightmapWidth-1)*mInfo.CellSpacing; // Returns total terrain width
}

float Terrain::GetDepth() const
{
	return (mInfo.HeightmapHeight-1)*mInfo.CellSpacing; // Returns total terrain depth
}

float Terrain::GetHeight(float x, float z) const
{
	// From local terrain space to cell space
	float c = (x + 0.5f*GetWidth()) /  mInfo.CellSpacing;
	float d = (z - 0.5f*GetDepth()) / -mInfo.CellSpacing;

	// Get row and column we are in
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	// Get heights of cell we are in
	float A = mHeightmap[row*mInfo.HeightmapWidth + col];
	float B = mHeightmap[row*mInfo.HeightmapWidth + col + 1];
	float C = mHeightmap[(row+1)*mInfo.HeightmapWidth + col];
	float D = mHeightmap[(row+1)*mInfo.HeightmapWidth + col + 1];

	// Where we are relative to the cell
	float s = c - (float)col;
	float t = d - (float)row;

	// Upper triangle ABC
	if( s + t <= 1.0f)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s*uy + t*vy;
	}
	else // Lower triangle DCB
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f-s)*uy + (1.0f-t)*vy;
	}
}

XMMATRIX Terrain::GetWorld()const
{
	return XMLoadFloat4x4(&mWorld);
}

void Terrain::Init(ID3D11Device* device, ID3D11DeviceContext* dc, const InitInfo& initInfo)
{
	mInfo = initInfo;

	// Divide heightmap into patches so that each patch has as many as CellsPerPatch
	mNumPatchVertRows = ((mInfo.HeightmapHeight-1) / CellsPerPatch) + 1;
	mNumPatchVertCols = ((mInfo.HeightmapWidth-1) / CellsPerPatch) + 1;

	// Calculate number of patch vertices and quad patch faces
	mNumPatchVertices  = mNumPatchVertRows*mNumPatchVertCols;
	mNumPatchQuadFaces = (mNumPatchVertRows-1)*(mNumPatchVertCols-1);

    LoadHeightmap();
	Smooth();

	BuildQuadPatchVB(device);
	BuildQuadPatchIB(device);
	BuildHeightmapSRV(device);

	std::vector<std::wstring> layerFilenames;
	layerFilenames.push_back(mInfo.LayerMapFilename0);
	layerFilenames.push_back(mInfo.LayerMapFilename1);
	layerFilenames.push_back(mInfo.LayerMapFilename2);
	layerFilenames.push_back(mInfo.LayerMapFilename3);
	layerFilenames.push_back(mInfo.LayerMapFilename4);
	mLayerMapArraySRV = d3dHelper::CreateTexture2DArraySRV(device, dc, layerFilenames);

	HR(D3DX11CreateShaderResourceViewFromFile(device, 
		mInfo.BlendMapFilename.c_str(), 0, 0, &mBlendMapSRV, 0));
}

void Terrain::Draw(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3])
{
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	dc->IASetInputLayout(InputLayouts::TerrainNormal);

	UINT stride = sizeof(Vertex::TerrainNormal);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, &mQuadPatchVB, &stride, &offset);
	dc->IASetIndexBuffer(mQuadPatchIB, DXGI_FORMAT_R16_UINT, 0);

	XMMATRIX viewProj = cam.getViewProjMatrix();
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX worldViewProj = world*viewProj;

	XMFLOAT4 worldPlanes[6];
	ExtractFrustumPlanes(worldPlanes, viewProj);

	// Set per frame constants.
	Effects::TerrainFX->SetViewProj(viewProj);
	Effects::TerrainFX->SetEyePosW(cam.getPosition());
	Effects::TerrainFX->SetDirLights(lights);
	Effects::TerrainFX->SetMinTessDistance(20.0f);
	Effects::TerrainFX->SetMaxTessDistance(500.0f);
	Effects::TerrainFX->SetMinTessFactor(0.0f);
	Effects::TerrainFX->SetMaxTessFactor(6.0f);
	Effects::TerrainFX->SetTexelCellSpaceU(1.0f / mInfo.HeightmapWidth);
	Effects::TerrainFX->SetTexelCellSpaceV(1.0f / mInfo.HeightmapHeight);
	Effects::TerrainFX->SetWorldCellSpace(mInfo.CellSpacing);
	Effects::TerrainFX->SetWorldFrustumPlanes(worldPlanes);

 	Effects::TerrainFX->SetLayerMapArray(mLayerMapArraySRV);
	Effects::TerrainFX->SetBlendMap(mBlendMapSRV);
	Effects::TerrainFX->SetHeightMap(mHeightMapSRV);

	Effects::TerrainFX->SetMaterial(mMat);

	Effects::TerrainFX->setFogColor(Colors::Silver);
	Effects::TerrainFX->setFogStart(30.0f);
	Effects::TerrainFX->setFogRange(300.0f);

	ID3DX11EffectTechnique* tech = Effects::TerrainFX->DirLights3FogTech;
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	for (UINT i = 0; i < techDesc.Passes; ++i)
	{
		ID3DX11EffectPass* pass = tech->GetPassByIndex(i);
		pass->Apply(0, dc);

		dc->DrawIndexed(mNumPatchQuadFaces*4, 0, 0);
	}

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	dc->HSSetShader(0, 0, 0);
	dc->DSSetShader(0, 0, 0);
}

// Generate quad patch vertex buffer
void Terrain::BuildQuadPatchVB(ID3D11Device* device)
{
	// Create vector of terrain vertices with size of number of patch vertices
	//std::vector<Vertex::TerrainNormal> patchVertices(mNumPatchVertRows * mNumPatchVertCols);

	mPatchVertices.resize(mNumPatchVertRows * mNumPatchVertCols);

	float halfWidth = 0.5f*GetWidth();
	float halfDepth = 0.5f*GetDepth();

	// Get patch properties
	float patchWidth = GetWidth() / (mNumPatchVertCols-1);
	float patchDepth = GetDepth() / (mNumPatchVertRows-1);
	float du = 1.0f / (mNumPatchVertCols-1);
	float dv = 1.0f / (mNumPatchVertRows-1);

	// Iterate through patch vertex rows
	for (UINT i = 0; i < mNumPatchVertRows; ++i)
	{
		float z = halfDepth - i*patchDepth;

		// Iterate through patch vertex columns
		for (UINT j = 0; j < mNumPatchVertCols; ++j)
		{
			float x = -halfWidth + j*patchWidth;

			mPatchVertices[i*mNumPatchVertCols+j].position = XMFLOAT3(x, 0.0f, z);

			// Stretch texture over grid
			mPatchVertices[i*mNumPatchVertCols+j].texCoord.x = j*du;
			mPatchVertices[i*mNumPatchVertCols+j].texCoord.y = i*dv;
		}
	}

	// Set bounding box y-bounds to upper-left patch corner
	/*
	for (UINT i = 0; i < mNumPatchVertRows-1; ++i)
	{
		for(UINT j = 0; j < mNumPatchVertCols-1; ++j)
		{
			UINT patchID = i*(mNumPatchVertCols-1)+j;
			patchVertices[i*mNumPatchVertCols+j].BoundsY = mPatchBoundsY[patchID];
		}
	}
	*/

	// Create buffer description
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::TerrainNormal) * mPatchVertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// Create buffer
	D3D11_SUBRESOURCE_DATA vInitData;
	vInitData.pSysMem = &mPatchVertices[0];
	HR(device->CreateBuffer(&vbd, &vInitData, &mQuadPatchVB));
}

void Terrain::BuildQuadPatchIB(ID3D11Device* device)
{
	// 4 indices per quad face
	std::vector<USHORT> indices(mNumPatchQuadFaces*4);

	// Iterate over each quad and compute indices
	int k = 0;
	for (UINT i = 0; i < mNumPatchVertRows-1; ++i)
	{
		for (UINT j = 0; j < mNumPatchVertCols-1; ++j)
		{
			// Top row
			indices[k] = i*mNumPatchVertCols+j;
			indices[k+1] = i*mNumPatchVertCols+j+1;

			// Bottom row
			indices[k+2] = (i+1)*mNumPatchVertCols+j;
			indices[k+3] = (i+1)*mNumPatchVertCols+j+1;

			// Next quad
			k += 4;
		}
	}

	// Create buffer description
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(USHORT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	// Create buffer
	D3D11_SUBRESOURCE_DATA iInitData;
	iInitData.pSysMem = &indices[0];
	HR(device->CreateBuffer(&ibd, &iInitData, &mQuadPatchIB));
}

void Terrain::LoadHeightmap()
{
	// Create temporary vector with heightmap size
	std::vector<unsigned char> in(mInfo.HeightmapWidth * mInfo.HeightmapHeight);

	// Open file
	std::ifstream inFile;
	inFile.open(mInfo.HeightMapFilename.c_str(), std::ios_base::binary);

	if (inFile)
	{
		// Read RAW bytes
		inFile.read((char*)&in[0], (std::streamsize)in.size());

		// Done, close it
		inFile.close();
	}

	// Resize heightmap vector
	mHeightmap.resize(mInfo.HeightmapHeight * mInfo.HeightmapWidth, 0);

	// Copy data to our heightmap vector
	for (UINT i = 0; i < mInfo.HeightmapHeight * mInfo.HeightmapWidth; ++i)
	{
		mHeightmap[i] = (in[i] / 255.0f)*mInfo.HeightScale;
	}
}

bool Terrain::InBounds(int i, int j)
{
	// Return true if there are 8 neighboring indices
	return i >= 0 && i < (int)mInfo.HeightmapHeight &&
		j >= 0 && j < (int)mInfo.HeightmapWidth;
}

float Terrain::Average(int i, int j)
{
	// Computes average height for chosen index
	// It uses an average of 8 neighboring indices
	float avg = 0.0f;
	float num = 0.0f;

	for (int m = i-1; m <= i+1; ++m)
	{
		for (int n = j-1; n <= j+1; ++n)
		{
			if (InBounds(m, n))
			{
				avg += mHeightmap[m*mInfo.HeightmapWidth + n];
				num += 1.0f;
			}
		}
	}

	return avg / num;
}

void Terrain::Smooth()
{
	std::vector<float> dest(mHeightmap.size());

	for(UINT i = 0; i < mInfo.HeightmapHeight; ++i)
	{
		for(UINT j = 0; j < mInfo.HeightmapWidth; ++j)
		{
			dest[i*mInfo.HeightmapWidth+j] = Average(i,j);
		}
	}

	// Now replace the heightmap with our smooth one
	mHeightmap = dest;
}

void Terrain::BuildHeightmapSRV( ID3D11Device* device )
{
	// Texture description
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = mInfo.HeightmapWidth;
	texDesc.Height = mInfo.HeightmapHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format    = DXGI_FORMAT_R16_FLOAT;
	texDesc.SampleDesc.Count   = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	// Store in a 16-bit float vector instead of 32-bit to save memory
	std::vector<HALF> hMap(mHeightmap.size());
	std::transform(mHeightmap.begin(), mHeightmap.end(), hMap.begin(), XMConvertFloatToHalf);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &hMap[0];
	data.SysMemPitch = mInfo.HeightmapWidth*sizeof(HALF);
	data.SysMemSlicePitch = 0;

	// Create texture
	ID3D11Texture2D* hMapTex = 0;
	HR(device->CreateTexture2D(&texDesc, &data, &hMapTex));

	// Now create the SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	HR(device->CreateShaderResourceView(hMapTex, &srvDesc, &mHeightMapSRV));

	// Release texture (SRV has a reference)
	ReleaseCOM(hMapTex);
}

void Terrain::ExtractFrustumPlanes( XMFLOAT4 planes[6], CXMMATRIX M )
{
	//
	// Left
	//
	planes[0].x = M(0,3) + M(0,0);
	planes[0].y = M(1,3) + M(1,0);
	planes[0].z = M(2,3) + M(2,0);
	planes[0].w = M(3,3) + M(3,0);

	//
	// Right
	//
	planes[1].x = M(0,3) - M(0,0);
	planes[1].y = M(1,3) - M(1,0);
	planes[1].z = M(2,3) - M(2,0);
	planes[1].w = M(3,3) - M(3,0);

	//
	// Bottom
	//
	planes[2].x = M(0,3) + M(0,1);
	planes[2].y = M(1,3) + M(1,1);
	planes[2].z = M(2,3) + M(2,1);
	planes[2].w = M(3,3) + M(3,1);

	//
	// Top
	//
	planes[3].x = M(0,3) - M(0,1);
	planes[3].y = M(1,3) - M(1,1);
	planes[3].z = M(2,3) - M(2,1);
	planes[3].w = M(3,3) - M(3,1);

	//
	// Near
	//
	planes[4].x = M(0,2);
	planes[4].y = M(1,2);
	planes[4].z = M(2,2);
	planes[4].w = M(3,2);

	//
	// Far
	//
	planes[5].x = M(0,3) - M(0,2);
	planes[5].y = M(1,3) - M(1,2);
	planes[5].z = M(2,3) - M(2,2);
	planes[5].w = M(3,3) - M(3,2);

	// Normalize the plane equations.
	for(int i = 0; i < 6; ++i)
	{
		XMVECTOR v = XMPlaneNormalize(XMLoadFloat4(&planes[i]));
		XMStoreFloat4(&planes[i], v);
	}
}

void Terrain::DrawToShadowMap(ID3D11DeviceContext* dc, XMMATRIX* lightViewProj)
{
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	dc->IASetInputLayout(InputLayouts::TerrainNormal);

	UINT stride = sizeof(Vertex::TerrainNormal);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, &mQuadPatchVB, &stride, &offset);
	dc->IASetIndexBuffer(mQuadPatchIB, DXGI_FORMAT_R16_UINT, 0);

	ID3DX11EffectTechnique* tech = Effects::BuildShadowMapFX->TessBuildShadowMapTech;
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX worldViewProj = world * (*lightViewProj);
	XMMATRIX worldInvTranspose;

	Effects::BuildShadowMapFX->SetWorld(world);
	Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
	Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
	Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
	Effects::BuildShadowMapFX->SetMinTessDistance(20.0f);
	Effects::BuildShadowMapFX->SetMaxTessDistance(500.0f);
	Effects::BuildShadowMapFX->SetMinTessFactor(0.0f);
	Effects::BuildShadowMapFX->SetMaxTessFactor(6.0f);

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	for (UINT i = 0; i < techDesc.Passes; ++i)
	{
		ID3DX11EffectPass* pass = tech->GetPassByIndex(i);
		pass->Apply(0, dc);

		dc->DrawIndexed(mNumPatchQuadFaces*4, 0, 0);
	}

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	dc->HSSetShader(0, 0, 0);
	dc->DSSetShader(0, 0, 0);
}

void Terrain::DrawShadowed(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3],
	ID3D11ShaderResourceView* shadowMap, XMMATRIX* shadowTransform)
{
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	dc->IASetInputLayout(InputLayouts::TerrainNormal);

	UINT stride = sizeof(Vertex::TerrainNormal);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, &mQuadPatchVB, &stride, &offset);
	dc->IASetIndexBuffer(mQuadPatchIB, DXGI_FORMAT_R16_UINT, 0);

	XMMATRIX viewProj = cam.getViewProjMatrix();
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX worldViewProj = world*viewProj;

	XMFLOAT4 worldPlanes[6];
	ExtractFrustumPlanes(worldPlanes, viewProj);

	// Set per frame constants.
	Effects::TerrainFX->SetViewProj(viewProj);
	Effects::TerrainFX->SetEyePosW(cam.getPosition());
	Effects::TerrainFX->SetDirLights(lights);
	Effects::TerrainFX->SetMinTessDistance(20.0f);
	Effects::TerrainFX->SetMaxTessDistance(500.0f);
	Effects::TerrainFX->SetMinTessFactor(0.0f);
	Effects::TerrainFX->SetMaxTessFactor(6.0f);
	Effects::TerrainFX->SetTexelCellSpaceU(1.0f / mInfo.HeightmapWidth);
	Effects::TerrainFX->SetTexelCellSpaceV(1.0f / mInfo.HeightmapHeight);
	Effects::TerrainFX->SetWorldCellSpace(mInfo.CellSpacing);
	Effects::TerrainFX->SetWorldFrustumPlanes(worldPlanes);

	Effects::TerrainFX->SetLayerMapArray(mLayerMapArraySRV);
	Effects::TerrainFX->SetBlendMap(mBlendMapSRV);
	Effects::TerrainFX->SetHeightMap(mHeightMapSRV);

	Effects::TerrainFX->SetMaterial(mMat);

	Effects::TerrainFX->setFogColor(Colors::Silver);
	Effects::TerrainFX->setFogStart(GameSettings::Instance()->Fog()->fogStart);
	Effects::TerrainFX->setFogRange(GameSettings::Instance()->Fog()->fogRange);

	Effects::TerrainFX->setShadowMap(shadowMap);
	Effects::TerrainFX->setShadowTransform(*shadowTransform);

	ID3DX11EffectTechnique* tech = Effects::TerrainFX->DirLights3FogTech;
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	for (UINT i = 0; i < techDesc.Passes; ++i)
	{
		ID3DX11EffectPass* pass = tech->GetPassByIndex(i);
		pass->Apply(0, dc);

		dc->DrawIndexed(mNumPatchQuadFaces*4, 0, 0);
	}

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	dc->HSSetShader(0, 0, 0);
	dc->DSSetShader(0, 0, 0);

}

std::vector<Vertex::TerrainNormal> Terrain::getPatchVertices() const
{
	return mPatchVertices;
}
