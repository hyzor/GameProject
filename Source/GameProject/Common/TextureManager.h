//------------------------------------------------------------------------------------------
// File: TextureManager.h
//
// This class is used to avoid loading duplicate textures
//------------------------------------------------------------------------------------------

#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_

#include <d3dUtilities.h>
#include <map>

class TextureManager
{
public:
	TextureManager(void);
	~TextureManager(void);

	void Init(ID3D11Device* device);

	ID3D11ShaderResourceView* CreateTexture(std::wstring fileName);

private:
	ID3D11Device* md3dDevice;
	std::map<std::wstring, ID3D11ShaderResourceView*> mTextureSRV;
};

#endif