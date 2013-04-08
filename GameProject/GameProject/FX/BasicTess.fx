//---------------------------------------------------------------------------
// A basic effect including texturing, lightning, fogging and shadow-mapping
//---------------------------------------------------------------------------

#include "LightDef.fx"

cbuffer cbPerFrame
{
	PointLight gPointLight;
	DirectionalLight gDirLights[3];

	float3 gEyePosW;

	// Tessellation intensity variables
	float gMinDist; // Tess = max
	float gMaxDist; // Tess = min

	// Tessellation range
	float gMinTess;
	float gMaxTess;

	// Fogging
	float gFogStart;
	float gFogRange;
	float4 gFogColor;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gWorldViewProjTex;
	float4x4 gTexTransform;
	float4x4 gShadowTransform;
	Material gMaterial;
};

Texture2D gDiffuseMap;
Texture2D gNormalMap;
Texture2D gShadowMap;
TextureCube gCubeMap;

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerComparisonState samShadow
{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;

	// Return 0 for points outside the light frustum to put in shadow
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	ComparisonFunc = LESS_EQUAL;
};

// Calculate how much to subdivide each patch
// Function used in the constant hull shader
float CalcTessFactor(float3 p)
{
	float d = distance(p, gEyePosW);

	float s = saturate((d-gMinDist) / (gMaxDist - gMinDist));

	return pow(2, (lerp(gMaxTess, gMinTess, s)));
}

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
	float4 TangentL : TANGENT;
};

struct VertexOut
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
	float4 TangentL : TANGENT;
};

VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;

	vOut.PosL = vIn.PosL;
	vOut.NormalL = vIn.NormalL;
	vOut.Tex = vIn.Tex;
	vOut.TangentL = vIn.TangentL;

	return vOut;
}

struct PatchTess
{
	float EdgeTess[3] : SV_TessFactor;
	float InsideTess : SV_InsideTessFactor;
};

//===========================================================================
// Constant hull shader
//===========================================================================
// Responsible for calculating how much to subdivide each patch
PatchTess ConstantHS(InputPatch<VertexOut, 3> patch,
					uint patchID : SV_PrimitiveID)
{
	PatchTess pt;

	// Find local and world space center coordinates
	float3 centerL = 0.333333f*(patch[0].PosL + patch[1].PosL + patch[2].PosL);
	float3 centerW = mul(float4(centerL, 1.0f), gWorld).xyz;

	// Get distance from eye
	//float d = distance(centerW, gEyePosW);

	// Tessellate patch based on distance from eye
	// gMinDist = max tess factor, gMaxDist = min tess factor
	//float tess = 64.0f * saturate((gMaxDist - d) / (gMaxDist - gMinDist));

	float tess = CalcTessFactor(centerW);

	// Tessellate patch
	pt.EdgeTess[0] = tess;
	pt.EdgeTess[1] = tess;
	pt.EdgeTess[2] = tess;
	
	pt.InsideTess = tess;
	
	return pt;
}

struct HullOut
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
	float4 TangentL : TANGENT;
};

//===========================================================================
// Hull shader
//===========================================================================
// This Hull Shader part is commonly used for a coordinate basis change, 
// for example changing from a quad to a Bezier bi-cubic.
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HullOut HS(InputPatch<VertexOut, 3> p, 
           uint i : SV_OutputControlPointID,
           uint patchId : SV_PrimitiveID)
{
	HullOut hOut;
	
	hOut.PosL = p[i].PosL;
	hOut.NormalL = p[i].NormalL;
	hOut.Tex = p[i].Tex;
	hOut.TangentL = p[i].TangentL;
	
	return hOut;
}

struct DomainOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 Tex : TEXCOORD0;
	float4 TangentW : TANGENT;
	float4 ShadowPosH : TEXCOORD1;
};

//===========================================================================
// Domain shader
//===========================================================================
// This shader is evaluated for each generated vertex
[domain("tri")]
DomainOut DS(PatchTess patchTess,
			 float3 baryCoords : SV_DomainLocation,
			 const OutputPatch<HullOut, 3> tri)
{
	DomainOut dOut;

	float3 pos = baryCoords.x * tri[0].PosL + baryCoords.y * tri[1].PosL + baryCoords.z * tri[2].PosL;

	// Displacement mapping
	//pos.y = 0;
	//pos.y = 0.3f*(pos.z*sin(pos.x) + pos.x*cos(pos.z));

	dOut.PosW = mul(float4(pos, 1.0f), gWorld).xyz;

	dOut.PosH = mul(float4(pos, 1.0f), gWorldViewProj);

	float3 normal = baryCoords.x * tri[0].NormalL + baryCoords.y * tri[1].NormalL + baryCoords.z * tri[2].NormalL;
	//float3 normal = (tri[0].NormalL + tri[1].NormalL + tri[2].NormalL) * 0.333333;

	//float3 normal = tri[0].NormalL;

	dOut.NormalW = mul(normal, (float3x3)gWorldInvTranspose);

	//dOut.Tex = baryCoords.x * tri[0].Tex + baryCoords.y * tri[1].Tex + baryCoords.z * tri[2].Tex;

	//float2 tex = (tri[0].Tex + tri[1].Tex + tri[2].Tex) * 0.333333;
	float2 tex = baryCoords.x * tri[0].Tex + baryCoords.y * tri[1].Tex + baryCoords.z * tri[2].Tex;

	dOut.Tex = mul(float4(tex, 0.0f, 1.0f), gTexTransform).xy;

	float4 tangent = baryCoords.x * tri[0].TangentL + baryCoords.y * tri[1].TangentL + baryCoords.z * tri[2].TangentL;
	dOut.TangentW = mul(tangent, gWorld);

	// Generate projective tex coords to project shadow map onto scene
	dOut.ShadowPosH = mul(float4(pos, 1.0f), gShadowTransform);

	return dOut;
}

//===========================================================================
// Pixel shader with directional lights and shadows
//===========================================================================
float4 PS_DirLight(DomainOut pIn, 
          uniform int gLightCount, 
		  uniform bool gUseTexture, 
		  uniform bool gAlphaClip, 
		  uniform bool gReflectionEnabled,
		  uniform bool gFogEnabled) : SV_Target
{
	// Normalize normal
    pIn.NormalW = normalize(pIn.NormalW);

	// The toEye vector is used in lighting
	float3 toEye = gEyePosW - pIn.PosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize
	toEye /= distToEye;
	
	float4 texColor = float4(1, 1, 1, 1);
	if (gUseTexture)
	{
		// Sample texture
		texColor = gDiffuseMap.Sample(samLinear, pIn.Tex);

		if (gAlphaClip)
		{
			// Discard pixel if alpha < 0.1
			clip(texColor.a - 0.1f);
		}
	}

	//--------------------------------------------------------
	// Lighting
	//--------------------------------------------------------
	float4 litColor = texColor;
	if (gLightCount > 0)
	{
	    // Initialize values
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

		float3 shadow = float3(1.0f, 1.0f, 1.0f);
		shadow[0] = CalcShadowFactor(samShadow, gShadowMap, pIn.ShadowPosH);

		// Sum light contribution from each light source
		[unroll]
		for (int i = 0; i < gLightCount; ++i)
		{
			float4 A, D, S;
			ComputeDirectionalLight(gMaterial, gDirLights[i], pIn.NormalW, toEye, A, D, S);

			ambient += A;
			diffuse += shadow[i]*D;
			specular += shadow[i]*S;
		}

		litColor = texColor * (ambient + diffuse) + specular;

		if (gReflectionEnabled)
		{
			float3 incident = -toEye;
			float3 reflectionVector = reflect(incident, pIn.NormalW);
			float4 reflectionColor = gCubeMap.Sample(samAnisotropic, reflectionVector);

			litColor += gMaterial.Reflect * reflectionColor;
		}
	}

	//--------------------------------------------------------
	// Fogging
	//--------------------------------------------------------
	if (gFogEnabled)
	{
		float fogLerp = saturate((distToEye - gFogStart) / gFogRange);

		// Blend fog color and lit color
		litColor = lerp(litColor, gFogColor, fogLerp);
	}

	litColor.a = gMaterial.Diffuse.a * texColor.a;

	return litColor;
}

technique11 TessDirLights3
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
        SetHullShader(CompileShader(hs_5_0, HS()));
        SetDomainShader(CompileShader(ds_5_0, DS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_DirLight(3, false, false, false, false)));
	}
}

technique11 TessDirLights3FogTex
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
        SetHullShader(CompileShader(hs_5_0, HS()));
        SetDomainShader(CompileShader(ds_5_0, DS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_DirLight(3, true, false, false, true)));
	}
}