//-------------------------------------------------------------------------
// This effect use the normal mapping technique on both basic models and
// skinned (animated) models.
// Also supports shadow mapping, reflection, texturing, lighting
// To do: SSAO
//-------------------------------------------------------------------------

#include "LightDef.fx"

cbuffer cbPerFrame
{
	DirectionalLight gDirLights[3];

	float3 gEyePosW;

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

cbuffer cbSkinned
{
	float4x4 gBoneTransforms[96];
};

Texture2D gShadowMap;
Texture2D gDiffuseMap;
Texture2D gNormalMap;
Texture2D gSsaoMap;

TextureCube gCubeMap;

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerComparisonState samShadow
{
	Filter   = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    ComparisonFunc = LESS_EQUAL;
};

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
	float4 TangentL : TANGENT;
};

struct SkinnedVertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
	float4 TangentL : TANGENT;
	float3 Weights : WEIGHTS;
	uint4 BoneIndices : BONEINDICES;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : SPOSITION;
	float3 NormalW : NORMAL;
	float2 Tex : TEXCOORD;
	float4 TangentW : TANGENT;
	float4 ShadowPosH : TEXCOORD1;
};

VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;

	// Transform to world space space
	vOut.PosW = mul(float4(vIn.PosL, 1.0f), gWorld).xyz;
	vOut.NormalW = mul(vIn.NormalL, (float3x3)gWorldInvTranspose);
	vOut.TangentW = mul(vIn.TangentL, gWorld);

	// Transform to homogeneous clip space
	vOut.PosH = mul(float4(vIn.PosL, 1.0f), gWorldViewProj);

	// Output vertex attributes for interpolation across triangle
	vOut.Tex = mul(float4(vIn.Tex, 0.0f, 1.0f), gTexTransform).xy;

	// Generate projective tex coords to project shadow map onto scene
	vOut.ShadowPosH = mul(float4(vIn.PosL, 1.0f), gShadowTransform);

	return vOut;
}

VertexOut SkinnedVS(SkinnedVertexIn vIn)
{
	VertexOut vOut;

	// Init weights
	float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	weights[0] = vIn.Weights.x;
	weights[1] = vIn.Weights.y;
	weights[2] = vIn.Weights.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	// Calculate position, normal and tangent space
	float3 posL = float3(0.0f, 0.0f, 0.0f);
	float3 normalL = float3(0.0f, 0.0f, 0.0f);
	float3 tangentL = float3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 4; ++i)
	{
		posL += weights[i] * mul(float4(vIn.PosL, 1.0f), gBoneTransforms[vIn.BoneIndices[i]]).xyz;
		normalL += weights[i] * mul(vIn.NormalL, (float3x3)gBoneTransforms[vIn.BoneIndices[i]]);
		tangentL += weights[i] * mul(vIn.TangentL.xyz, (float3x3)gBoneTransforms[vIn.BoneIndices[i]]);
	}

	// Now transform them to world space space
	vOut.PosW = mul(float4(posL, 1.0f), gWorld).xyz;
	vOut.NormalW = mul(normalL, (float3x3)gWorldInvTranspose);
	vOut.TangentW = float4(mul(tangentL, (float3x3)gWorld), vIn.TangentL.w);

	// Transform to homogeneous clip space
	vOut.PosH = mul(float4(posL, 1.0f), gWorldViewProj);

	// Output vertex attributes for interpolation across triangle
	vOut.Tex = mul(float4(vIn.Tex, 0.0f, 1.0f), gTexTransform).xy;

	// Generate projective tex coords to project shadow map onto scene
	vOut.ShadowPosH = mul(float4(posL, 1.0f), gShadowTransform);

	return vOut;
}

//===========================================================================
// Pixel shader with point lights and shadows
//===========================================================================
float4 PS_DirLight(VertexOut pIn, 
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
	// Normal mapping
	//--------------------------------------------------------
	float3 normalMapSample = gNormalMap.Sample(samLinear, pIn.Tex).rgb;
	float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample, pIn.NormalW, pIn.TangentW);

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

		[unroll]
		for (int i = 0; i < gLightCount; ++i)
		{
			float4 A, D, S;
			ComputeDirectionalLight(gMaterial, gDirLights[i], bumpedNormalW, toEye, A, D, S);

			ambient += A;
			diffuse += shadow[i]*D;
			specular += shadow[i]*S;
		}

		litColor = texColor * (ambient + diffuse) + specular;

		if (gReflectionEnabled)
		{
			float3 incident = -toEye;
			float3 reflectionVector = reflect(incident, bumpedNormalW);
			float4 reflectionColor = gCubeMap.Sample(samLinear, reflectionVector);

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

technique11 DirLights3Tex
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_DirLight(3, true, false, false, false)));
	}
}

technique11 DirLights3TexSkinned
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_DirLight(3, true, false, false, false)));
	}
}