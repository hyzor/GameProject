//------------------------------------------------------------------------------------------
// File: TextureManager.cpp
//
// This class is used to avoid loading duplicate textures
//------------------------------------------------------------------------------------------

#include "TextureManager.h"

TextureManager::TextureManager(void)
	: md3dDevice(0)
{
}


TextureManager::~TextureManager(void)
{
	for(auto it = mTextureSRV.begin(); it != mTextureSRV.end(); ++it)
	{
		ReleaseCOM(it->second);
	}

	mTextureSRV.clear();
}

void TextureManager::Init(ID3D11Device* device)
{
	md3dDevice = device;
}

ID3D11ShaderResourceView* TextureManager::CreateTexture(std::wstring fileName)
{
	ID3D11ShaderResourceView* srv = 0;

	// Find if texture already exists
	if (mTextureSRV.find(fileName) != mTextureSRV.end())
	{
		srv = mTextureSRV[fileName]; // Just point to existing texture
	}

	// Otherwise create the new texture
	else
	{
		HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, fileName.c_str(), 0, 0, &srv, 0));

		mTextureSRV[fileName] = srv;
	}

	return srv;
}
