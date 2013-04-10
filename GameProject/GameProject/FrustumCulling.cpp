#include "FrustumCulling.h"


FrustumCulling::FrustumCulling(void)
{
	mFrustumCullingEnabled = true;
	mNumVisible = 0;
}


FrustumCulling::~FrustumCulling(void)
{
}

void FrustumCulling::frustumCull(std::vector<GenericModelInstance>& instances, Camera& camera)
{
	if (!mFrustumCullingEnabled)
		return;

	mNumVisible = 0;

	XMVECTOR detView = XMMatrixDeterminant(camera.getViewMatrix());
	XMMATRIX invView = XMMatrixInverse(&detView, camera.getViewMatrix());

	for (UINT i = 0; i < instances.size(); ++i)
	{
		instances[i].isVisible = false;

		XMMATRIX W = XMLoadFloat4x4(&instances[i].world);
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

		// View space to the object's local space.
		XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

		// Decompose the matrix into its individual parts.
		XMVECTOR scale;
		XMVECTOR rotQuat;
		XMVECTOR translation;
		XMMatrixDecompose(&scale, &rotQuat, &translation, toLocal);

		// Transform the camera frustum from view space to the object's local space.
		XNA::Frustum localspaceFrustum;
		XNA::TransformFrustum(&localspaceFrustum, &camera.getFrustum(), XMVectorGetX(scale), rotQuat, translation);

		// Perform the box/frustum intersection test in local space.
		if(XNA::IntersectAxisAlignedBoxFrustum(&instances[i].model->boundingBox, &localspaceFrustum) != 0)
		{
			// Write the instance data to dynamic VB of the visible objects.
			//dataView[mVisibleObjectCount++] = mInstancedData[i];
			mNumVisible++;
			instances[i].isVisible = true;
		}
	}
}

UINT FrustumCulling::getNumVisible() const
{
	return mNumVisible;
}
