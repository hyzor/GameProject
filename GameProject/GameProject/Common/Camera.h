#ifndef CAMERA_H
#define CAMERA_H

#include "d3dUtilities.h"

class Camera
{
public:
	Camera(void);
	~Camera(void);

	// Camera position functions
	XMVECTOR getPositionXM() const;
	XMFLOAT3 getPosition() const;

	void setPosition(float x, float y, float z);
	void setPosition(const XMFLOAT3& v);

	// Get functions
	XMVECTOR getRightXM() const;
	XMFLOAT3 getRight() const;
	XMVECTOR getUpXM() const;
	XMFLOAT3 getUp() const;
	XMVECTOR getLookXM() const;
	XMFLOAT3 getLook() const;

	// Get frustum (lens) properties
	float getNearZ() const;
	float getFarZ() const;
	float getAspect() const;
	float getFovY() const;
	float getFovX() const;

	// Set frustum (lens)
	void setLens(float fovY, float aspect, float zn, float zf);

	// Get matrices
	XMMATRIX getViewMatrix() const;
	XMMATRIX getProjMatrix() const;
	XMMATRIX getViewProjMatrix() const;

	// Get near and far plane dimensions in view space coordinates
	float getNearWindowWidth() const;
	float getNearWindowHeight() const;
	float getFarWindowWidth() const;
	float getFarWindowHeight() const;

	void updateViewMatrix();

	void walk(float dist);
	void strafe(float dist);

	void pitch(float angle);
	void yaw(float angle);

private:
	// Coordinate system relative to world space
	XMFLOAT3 mPosition; // view space origin
	XMFLOAT3 mRight; // view space x-axis
	XMFLOAT3 mUp; // view space y-axis
	XMFLOAT3 mLook; // view space z-axis

	// Matrix cache
	XMFLOAT4X4 mView; // View matrix
	XMFLOAT4X4 mProj; // Projection matrix

	// Frustum cache
	float mNearZ;
	float mFarZ;
	float mAspect;
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;
};

#endif