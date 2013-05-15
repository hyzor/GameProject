//---------------------------------------------------------------------------
// A basic effect including texturing, lightning, fogging and shadow-mapping
//---------------------------------------------------------------------------

#include "LightDef.fx"

cbuffer cbPerFrame
{
	PointLight gPointLight;
	DirectionalLight gDirLights[3];

	float3 gEyePosW;

	// Fogging
	float gFogStart;
	float gFogRange;
	float4 gFogColor;

	bool inMenu;
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

	ComparisonFunc = LESS;
};

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 Tex : TEXCOORD0;
	float4 ShadowPosH : TEXCOORD1;
};

VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;

	// Transform to world space space
	vOut.PosW = mul(float4(vIn.PosL, 1.0f), gWorld).xyz;
	vOut.NormalW = mul(vIn.NormalL, (float3x3)gWorldInvTranspose);

	// Transform to homogeneous clip space
	vOut.PosH = mul(float4(vIn.PosL, 1.0f), gWorldViewProj);

	// Output vertex attributes for interpolation across triangle
	vOut.Tex = mul(float4(vIn.Tex, 0.0f, 1.0f), gTexTransform).xy;

	// Generate projective tex coords to project shadow map onto scene
	vOut.ShadowPosH = mul(float4(vIn.PosL, 1.0f), gShadowTransform);

	return vOut;
}

//===========================================================================
// Pixel shader with point lights and shadows
//===========================================================================
float4 PS_PointLight(VertexOut pIn, 
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
		texColor = gDiffuseMap.Sample(samAnisotropic, pIn.Tex);

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

		[unroll]
		for (int i = 0; i < gLightCount; ++i)
		{
			float4 A, D, S;
			ComputePointLight(gMaterial, gPointLight, pIn.PosW, pIn.NormalW, toEye, A, D, S);

			ambient += A;
			diffuse += shadow[i]*D;
			specular += shadow[i]*S;
		}

		litColor = texColor * (ambient + diffuse) + specular;

		if (gReflectionEnabled)
		{
			float3 incident = -toEye;
			float3 reflectionVector = reflect(incident, pIn.NormalW);
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

//===========================================================================
// Pixel shader with directional lights and shadows
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
		if(inMenu)
		{
			texColor.rgb = dot(texColor.rgb, float3(0.3, 0.59, 0.11));
			texColor.rgb *= 0.5;
			texColor.a = 1;
		}
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

technique11 PointLight1
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_PointLight(1, false, false, false, false)));
	}
}

technique11 DirLights3
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_DirLight(3, false, false, false, false)));
	}
}

technique11 PointLight1Tex
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_PointLight(1, true, false, false, false)));
	}
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

technique11 DirLights3TexAlphaClip
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_DirLight(3, true, true, false, false)));
	}
}

technique11 DirLights3Fog
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_DirLight(3, false, false, false, true)));
	}
}

technique11 DirLights3FogTex
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_DirLight(3, true, false, false, true)));
	}
}

technique11 DirLights3FogReflection
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_DirLight(3, false, false, true, true)));
	}
}