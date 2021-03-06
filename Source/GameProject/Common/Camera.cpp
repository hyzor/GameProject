#include "Camera.h"


Camera::Camera(void)
	: mPosition(0.0f, 0.0f, 0.0f), 
	mRight(1.0f, 0.0f, 0.0f),
	mUp(0.0f, 1.0f, 0.0f),
	mLook(0.0f, 0.0f, 1.0f)
{
	SetLens(0.25f*MathHelper::pi, 1.0f, 1.0f, 1000.0f);
	Yaw = 0;
	Pitch = 0;
	Roll = 0;
}


Camera::~Camera(void)
{
}

XMVECTOR Camera::GetPositionXM() const
{
	return XMLoadFloat3(&mPosition);
}

XMFLOAT3 Camera::GetPosition() const
{
	return mPosition;
}

void Camera::SetPosition( float x, float y, float z )
{
	mPosition = XMFLOAT3(x, y, z);
}

void Camera::SetPosition( const XMFLOAT3& v )
{
	mPosition = v;
}

XMVECTOR Camera::GetRightXM() const
{
	return XMLoadFloat3(&mRight);
}

XMFLOAT3 Camera::GetRight() const
{
	return mRight;
}

XMVECTOR Camera::GetUpXM() const
{
	return XMLoadFloat3(&mUp);
}

XMFLOAT3 Camera::GetUp() const
{
	return mUp;
}

XMVECTOR Camera::GetLookXM() const
{
	return XMLoadFloat3(&mLook);
}

XMFLOAT3 Camera::GetLook() const
{
	return mLook;
}

XMMATRIX Camera::GetViewMatrix() const
{
	return XMLoadFloat4x4(&mView);
}

XMMATRIX Camera::GetProjMatrix() const
{
	return XMLoadFloat4x4(&mProj);
}

XMMATRIX Camera::GetViewProjMatrix() const
{
	return XMMatrixMultiply(GetViewMatrix(), GetProjMatrix());
}

#pragma region Frustum
float Camera::GetNearZ() const
{
	return mNearZ;
}

float Camera::GetFarZ() const
{
	return mFarZ;
}

float Camera::GetAspect() const
{
	return mAspect;
}

float Camera::GetFovY() const
{
	return mFovY;
}

float Camera::GetFovX() const
{
	float halfWidth = 0.5f*GetNearWindowWidth();
	return 2.0f*atan(halfWidth / mNearZ);
}

void Camera::SetLens( float fovY, float aspect, float zn, float zf )
{
	// Cache properties
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf( 0.5f*mFovY );
	mFarWindowHeight  = 2.0f * mFarZ * tanf( 0.5f*mFovY );

	XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&mProj, P);
}
#pragma endregion Frustum

float Camera::GetNearWindowWidth() const
{
	return mAspect * mNearWindowHeight;
}

float Camera::GetNearWindowHeight() const
{
	return mNearWindowHeight;
}

float Camera::GetFarWindowWidth() const
{
	return mAspect * mFarWindowHeight;
}

float Camera::GetFarWindowHeight() const
{
	return mFarWindowHeight;
}

void Camera::UpdateViewMatrix(XMVECTOR Up, XMVECTOR Look, XMVECTOR Right)
{
	XMVECTOR pos = XMLoadFloat3(&mPosition);

	/*XMVECTOR Up = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
	XMVECTOR Look = XMLoadFloat3(&XMFLOAT3(0, 0, 1));
	XMVECTOR Right = XMLoadFloat3(&XMFLOAT3(1, 0, 0));*/


	XMMATRIX yawMatrix = XMMatrixRotationAxis(Up, Yaw);
	Look = XMVector3TransformCoord(Look, yawMatrix);
	Right = XMVector3TransformCoord(Right, yawMatrix);

	XMMATRIX pitchMatrix = XMMatrixRotationAxis(Right, Pitch);
	Look = XMVector3TransformCoord(Look, pitchMatrix);
	Up = XMVector3TransformCoord(Up, pitchMatrix);

	XMMATRIX rollMatrix = XMMatrixRotationAxis(Look, Roll);
	Right = XMVector3TransformCoord(Right, rollMatrix);
	Up = XMVector3TransformCoord(Up, rollMatrix);


	mView(0,0) = XMVectorGetX(Right); mView(0,1) = XMVectorGetX(Up); mView(0,2) = XMVectorGetX(Look);
	mView(1,0) = XMVectorGetY(Right); mView(1,1) = XMVectorGetY(Up); mView(1,2) = XMVectorGetY(Look);
	mView(2,0) = XMVectorGetZ(Right); mView(2,1) = XMVectorGetZ(Up); mView(2,2) = XMVectorGetZ(Look);

	mView(3,0) = -XMVectorGetX(XMVector3Dot(pos, Right));
	mView(3,1) = -XMVectorGetX(XMVector3Dot(pos, Up));
	mView(3,2) = -XMVectorGetX(XMVector3Dot(pos, Look));
	
	mView(0,3) = 0;
	mView(1,3) = 0;
	mView(2,3) = 0;
	mView(3,3) = 1;

	XMStoreFloat3(&mRight, Right);
	XMStoreFloat3(&mUp, Up);
	XMStoreFloat3(&mLook, Look);
}


XNA::Frustum Camera::GetFrustum() const
{
	return mFrustum;
}

void Camera::ComputeFrustum()
{
	ComputeFrustumFromProjection(&mFrustum, &GetProjMatrix());
}
