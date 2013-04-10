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

	void frustumCull(std::vector<GenericModelInstance>& instances, Camera& camera);

	UINT getNumVisible() const;

private:
	bool mFrustumCullingEnabled;
	UINT mNumVisible;
};

#endif