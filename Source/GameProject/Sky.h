//------------------------------------------------------------------------------------
// File: Sky.h
//
// This class renders a sky using a cube map
//------------------------------------------------------------------------------------

#ifndef SKY_H_
#define SKY_H_

#include "d3dUtilities.h"
#include <Camera.h>

class Sky
{
public:
	Sky(ID3D11Device* device, const std::wstring& cubeMapFileName, float skySphereRadius);
	~Sky(void);

	ID3D11ShaderResourceView* cubeMapSRV();

	void draw(ID3D11DeviceContext* dc, const Camera& camera, bool inMenu);

private:
	// Buffers
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;

	// Texture
	ID3D11ShaderResourceView* mCubeMapSRV;

	UINT mIndexCount;
};

#endif