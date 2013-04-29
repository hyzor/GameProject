#ifndef FRUSTUMCULLING_H_
#define FRUSTUMCULLING_H_

#include <d3dUtilities.h>
#include "GenericModel.h"
#include <Camera.h>

class FrustumCulling
{
public:
	FrustumCulling(void);
	~FrustumCulling(void);

	void FrustumCull(std::vector<GenericModelInstance>& instances, Camera& camera);

	UINT GetNumVisible() const;

private:
	bool mFrustumCullingEnabled;
	UINT mNumVisible;
};

#endif