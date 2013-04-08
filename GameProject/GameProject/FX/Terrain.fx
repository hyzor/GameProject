#include "LightDef.fx"

cbuffer cbPerFrame
{
	float3 gEyePosW;

	DirectionalLight gDirLights[3];

	// Tessellation intensity variables
	float gMinDist; // Tess = max
	float gMaxDist; // Tess = min

	// Tessellation range
	float gMinTess;
	float gMaxTess;

	float2 gTexScale = 50.0f;

	// Spacing between height values in normalized uv-space
	float gTexelCellSpaceU;
	float gTexelCellSpaceV;

	// Spacing between cells in world space
	float gWorldCellSpace;

	float4 gWorldFrustumPlanes[6];

	// Fogging
	float gFogStart;
	float gFogRange;
	float4 gFogColor;
};

cbuffer cbPerObject
{
	float4x4 gViewProj;
	Material gMaterial;

	// FOR SHADOW MAP
	float4x4 gShadowTransform;
};

// Height map texture
Texture2D gHeightMap;

// Array for all texture layers
Texture2DArray gLayerMapArray;

// Blend map for blending multiple textures
Texture2D gBlendMap;

// FOR SHADOW MAP
Texture2D gShadowMap;

// FOR SHADOW MAP
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

SamplerState samHeightmap
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;

	AddressU = WRAP;
	AddressV = WRAP;
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
	float3 PosL : POSITION; // Local position
	//float2 Tex : TEXCOORD0;

	// FOR SHADOW MAP
	float3 NormalW : NORMAL;
	float2 Tex : TEXCOORD;

	float2 BoundsY : TEXCOORD1;
};

struct VertexOut
{
	float3 PosW : POSITION; // World position
	//float2 Tex : TEXCOORD0;

	// FOR SHADOW MAP
	float3 NormalW : NORMAL;
	float2 Tex : TEXCOORD;

	float2 BoundsY : TEXCOORD1;
};

//===========================================================================
// Vertex shader
//===========================================================================
VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;

	// Terrain already in world space
	vOut.PosW = vIn.PosL;

	// Displace patch corners to world space
	vOut.PosW.y = gHeightMap.SampleLevel(samHeightmap, vIn.Tex, 0).r;

	// Return attributes for next stage
	vOut.Tex = vIn.Tex;
	vOut.BoundsY = vIn.BoundsY;

	// FOR SHADOW MAP
	vOut.NormalW = vIn.NormalW;

	return vOut;
}

struct PatchTess
{
	float EdgeTess[4] : SV_TessFactor;
	float InsideTess[2] : SV_InsideTessFactor;
};

//===========================================================================
// Constant hull shader
//===========================================================================
// Responsible for calculating how much to subdivide each patch
PatchTess ConstantHS(InputPatch<VertexOut, 4> patch,
				uint patchID : SV_PrimitiveID)
{
	PatchTess pt;

	// Tessellation factor calculation based on edges
	// Compute midpoint on edges, and patch center
	float3 e0 = 0.5f*(patch[0].PosW + patch[2].PosW);
	float3 e1 = 0.5f*(patch[0].PosW + patch[1].PosW);
	float3 e2 = 0.5f*(patch[1].PosW + patch[3].PosW);
	float3 e3 = 0.5f*(patch[2].PosW + patch[3].PosW);
	float3  c = 0.25f*(patch[0].PosW + patch[1].PosW + patch[2].PosW + patch[3].PosW);
		
	pt.EdgeTess[0] = CalcTessFactor(e0);
	pt.EdgeTess[1] = CalcTessFactor(e1);
	pt.EdgeTess[2] = CalcTessFactor(e2);
	pt.EdgeTess[3] = CalcTessFactor(e3);
		
	pt.InsideTess[0] = CalcTessFactor(c);
	pt.InsideTess[1] = pt.InsideTess[0];
	
	return pt;
}

struct HullOut
{
	float3 PosW : POSITION;

	// FOR SHADOW MAP
	float3 NormalW : NORMAL;

	float2 Tex : TEXCOORD;
};

//===========================================================================
// Hull shader
//===========================================================================
[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HullOut HS(InputPatch<VertexOut, 4> p, uint i : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
	HullOut hOut;

	hOut.PosW = p[i].PosW;
	hOut.Tex = p[i].Tex;

	// FOR SHADOW MAP
	hOut.NormalW = p[i].NormalW;

	return hOut;
}

struct DomainOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	//float2 Tex : TEXCOORD0;

	// FOR SHADOW MAP
	float3 NormalW : NORMAL;
	float2 Tex : TEXCOORD;

	float2 TiledTex : TEXCOORD1;

	// FOR SHADOW MAPPING
	float4 ShadowPosH : TEXCOORD2;
};

//===========================================================================
// Domain shader
//===========================================================================
// This shader is evaluated for each generated vertex
// Here we use the u, v coordinates of tessellated vertex positions to interpolate control point data
// Also we perform displacement mapping
[domain("quad")]
DomainOut DS(PatchTess patchTess, float2 uv : SV_DomainLocation, const OutputPatch<HullOut, 4> quad)
{
	DomainOut dOut;

	// Bilinear interpolation
	dOut.PosW = lerp(
		lerp(quad[0].PosW, quad[1].PosW, uv.x),
		lerp(quad[2].PosW, quad[3].PosW, uv.x),
		uv.y);

	dOut.Tex = lerp(
		lerp(quad[0].Tex, quad[1].Tex, uv.x),
		lerp(quad[2].Tex, quad[3].Tex, uv.x),
		uv.y);

	// Tile layer textures over terrain
	dOut.TiledTex = dOut.Tex*gTexScale;

	// Displacement mapping
	dOut.PosW.y = gHeightMap.SampleLevel(samHeightmap, dOut.Tex, 0).r;

	dOut.PosH = mul(float4(dOut.PosW, 1.0f), gViewProj);

	// FOR SHADOW MAPPING
 	dOut.NormalW = lerp(
 		lerp(quad[0].NormalW, quad[1].NormalW, uv.x),
 		lerp(quad[2].NormalW, quad[3].NormalW, uv.x),
 		uv.y);

	// FOR SHADOW MAPPING
	// Generate projective tex coords to project shadow map onto scene
	dOut.ShadowPosH = mul(float4(dOut.PosW, 1.0f), gShadowTransform);

	return dOut;
}

//===========================================================================
// Pixel shader
//===========================================================================
float4 PS(DomainOut pIn,
			uniform int gLightCount,
			uniform bool gFogEnabled) : SV_TARGET
{
	// Estimate normal and tangent using central differences
	float2 leftTex   = pIn.Tex + float2(-gTexelCellSpaceU, 0.0f);
	float2 rightTex  = pIn.Tex + float2(gTexelCellSpaceU, 0.0f);
	float2 bottomTex = pIn.Tex + float2(0.0f, gTexelCellSpaceV);
	float2 topTex    = pIn.Tex + float2(0.0f, -gTexelCellSpaceV);
	
	float leftY   = gHeightMap.SampleLevel( samHeightmap, leftTex, 0 ).r;
	float rightY  = gHeightMap.SampleLevel( samHeightmap, rightTex, 0 ).r;
	float bottomY = gHeightMap.SampleLevel( samHeightmap, bottomTex, 0 ).r;
	float topY    = gHeightMap.SampleLevel( samHeightmap, topTex, 0 ).r;
	
	float3 tangent = normalize(float3(2.0f*gWorldCellSpace, rightY - leftY, 0.0f));
	float3 bitan   = normalize(float3(0.0f, bottomY - topY, -2.0f*gWorldCellSpace)); 
	float3 normalW = cross(tangent, bitan);

	// FOR SHADOW MAP
	pIn.NormalW = normalW;

	// Used for lightning
	float3 toEye = gEyePosW - pIn.PosW;

	float distToEye = length(toEye);

	// Normalize
	toEye /= distToEye;
	
	//-----------------------------------------------------------------
	// Texturing
	//-----------------------------------------------------------------

	// Sample layers in texture array
	float4 c0 = gLayerMapArray.Sample(samLinear, float3(pIn.TiledTex, 0.0f));
	float4 c1 = gLayerMapArray.Sample(samLinear, float3(pIn.TiledTex, 1.0f));
	float4 c2 = gLayerMapArray.Sample(samLinear, float3(pIn.TiledTex, 2.0f));
	float4 c3 = gLayerMapArray.Sample(samLinear, float3(pIn.TiledTex, 3.0f));
	float4 c4 = gLayerMapArray.Sample(samLinear, float3(pIn.TiledTex, 4.0f)); 

	// Sample blend map
	float4 t = gBlendMap.Sample(samLinear, pIn.Tex);

	// Blend the layers on top of each other.
    float4 texColor = c0;
    texColor = lerp(texColor, c1, t.r);
    texColor = lerp(texColor, c2, t.g);
    texColor = lerp(texColor, c3, t.b);
    texColor = lerp(texColor, c4, t.a);

	//-----------------------------------------------------------------
	// Lightning
	//-----------------------------------------------------------------

	float4 litColor = texColor;

	if (gLightCount > 0)
	{
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// FOR SHADOW MAPPING
		float3 shadow = float3(1.0f, 1.0f, 1.0f);
		shadow[0] = CalcShadowFactor(samShadow, gShadowMap, pIn.ShadowPosH);

		[unroll]
		for (int i=0; i < gLightCount; ++i)
		{
			float4 A, D, S;
			ComputeDirectionalLight(gMaterial, gDirLights[i], normalW, toEye,
			 A, D, S);

			ambient += A;
			//diffuse += D;
			//specular += S;

			// FOR SHADOW MAPPING
			diffuse += shadow[i]*D;
			specular += shadow[i]*S;
		}

		litColor = texColor*(ambient + diffuse) + specular;
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

	return litColor;
}

// Tech1 use three directional lights
technique11 Tech1
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetHullShader(CompileShader(hs_5_0, HS()));
		SetDomainShader( CompileShader(ds_5_0, DS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(3, false)));
	}
}

technique11 DirLights3
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetHullShader(CompileShader(hs_5_0, HS()));
		SetDomainShader( CompileShader(ds_5_0, DS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(3, false)));
	}
}

technique11 DirLights3Fog
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetHullShader(CompileShader(hs_5_0, HS()));
		SetDomainShader( CompileShader(ds_5_0, DS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(3, true)));
	}
}