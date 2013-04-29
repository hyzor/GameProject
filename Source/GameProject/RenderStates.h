#ifndef RENDERSTATES_H_
#define RENDERSTATES_H_

#include <d3dUtilities.h>

class RenderStates
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11RasterizerState* WireFrameRS;
};

#endif