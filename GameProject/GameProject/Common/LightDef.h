//-------------------------------------------------------------------------------------------
// File: LightDef.h
//
// This class is only used to define lights
//-------------------------------------------------------------------------------------------

#ifndef LIGHTDEF_H
#define LIGHTDEF_H

#include <windows.h>
#include <xnamath.h>

struct DirectionalLight
{
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Forms into a 4D vector
	XMFLOAT3 Direction;
	float Padding;
};

struct PointLight
{
	PointLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Forms into a 4D vector
	XMFLOAT3 Position;
	float Range;

	// Forms into a 4D vector
	XMFLOAT3 Attenuation;
	float Padding;
};

struct SpotLight
{
	SpotLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Forms into a 4D vector
	XMFLOAT3 Position;
	float Range;
	
	// Forms into a 4D vector
	XMFLOAT3 Direction;
	float Spot;

	// Forms into a 4D vector
	XMFLOAT3 Attenuation;
	float Padding;
};

struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular; // w = SpecPower
	XMFLOAT4 Reflect;
};

#endif