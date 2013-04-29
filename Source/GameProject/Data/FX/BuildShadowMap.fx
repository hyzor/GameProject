// The geometry we are drawing to the depth in our shadow map has to be
// tessellated exactly the same way when we are rendering it from the eye
// so the geometry in the shadow map is the same as when the eye sees it

cbuffer cbPerFrame
{
	float3 gEyePosW;

	float gHeightScale;
	float gMaxTessDistance;
	float gMinTessDistance;
	float gMinTessFactor;
	float gMaxTessFactor;
}

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gViewProj;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
}

Texture2D gDiffuseMap;
Texture2D gNormalMap;

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
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
	float2 Tex : TEXCOORD;
};

//============================================================================
// Vertex shader
//============================================================================
VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;

	vOut.PosH = mul(float4(vIn.PosL, 1.0f), gWorldViewProj);
	vOut.Tex = mul(float4(vIn.Tex, 0.0f, 1.0f), gTexTransform).xy;

	return vOut;
}

struct TessVertexOut
{
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 Tex : TEXCOORD;
	float TessFactor : TESS;
};

//============================================================================
// Vertex shader - Tessellation
//============================================================================
TessVertexOut TessVS(VertexIn vIn)
{
	TessVertexOut vOut;

	vOut.PosW = mul(float4(vIn.PosL, 1.0f), gWorld).xyz;
	vOut.NormalW = mul(vIn.NormalL, (float3x3)gWorldInvTranspose);
	vOut.Tex = mul(float4(vIn.Tex, 0.0f, 1.0f), gTexTransform).xy;

	float dist = distance(vOut.PosW, gEyePosW);

	// Normalized tessellation factor
	// If dist >= gMinTessDistance, tesselation = 0
	// If dist <= gMaxTessDistance, tesselation = 1
	float tess = saturate((gMinTessDistance - dist) / (gMinTessDistance - gMaxTessDistance));

	// Get the real tessellation factor
	vOut.TessFactor = gMinTessFactor + tess*(gMaxTessFactor - gMinTessFactor);

	return vOut;
}

// Contains data of how many triangles there are in each part of the patch (triangle)
struct PatchTess
{
	float EdgeTess[3] : SV_TessFactor;		// 3 edges (triangle)
	float InsideTess : SV_InsideTessFactor; // How many triangles in the inside of patch
};

//============================================================================
// Hull shader for each patch
//============================================================================
PatchTess PatchHS(InputPatch<TessVertexOut, 3> patch, uint patchID : SV_PrimitiveID)
{
	PatchTess patchTess;

	// Calculate the average tess factors along edges, so that the edges shared by
	// more than one triangle will have the same tessellation factor
	patchTess.EdgeTess[0] = 0.5f*(patch[1].TessFactor + patch[2].TessFactor);
	patchTess.EdgeTess[1] = 0.5f*(patch[2].TessFactor + patch[0].TessFactor);
	patchTess.EdgeTess[2] = 0.5f*(patch[0].TessFactor + patch[1].TessFactor);
	
	patchTess.InsideTess = patchTess.EdgeTess[0];

	return patchTess;
}

struct HullOut
{
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 Tex : TEXCOORD;
};

//============================================================================
// Hull shader
//============================================================================
// Set properties (set to triangle etc.)
[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchHS")]
HullOut HS(InputPatch<TessVertexOut, 3> p,
			uint i : SV_OutputControlPointID,
			uint patchID : SV_PrimitiveID)
{
	HullOut hOut;

	// Pass through vertex shader
	hOut.PosW = p[i].PosW;
	hOut.NormalW = p[i].NormalW;
	hOut.Tex = p[i].Tex;

	return hOut;
}

struct DomainOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 Tex : TEXCOORD;
};

//============================================================================
// Domain shader
//============================================================================
// Domain shader is called for every vertex created by the tessellator
[domain("tri")]
DomainOut DS(PatchTess patchTess, float3 bary : SV_DomainLocation,
				const OutputPatch<HullOut, 3> tri)
{
	DomainOut dOut;

	// Interpolate patch attributes to the generated vertices
	dOut.PosW		= bary.x*tri[0].PosW + bary.y*tri[1].PosW + bary.z*tri[2].PosW;
	dOut.NormalW	= bary.x*tri[0].NormalW + bary.y*tri[1].NormalW + bary.z*tri[2].NormalW;
	dOut.Tex		= bary.x*tri[0].Tex + bary.y*tri[1].Tex + bary.z*tri[2].Tex;

	// Interpolation normal can unnormalize it, so normalize it
	dOut.NormalW = normalize(dOut.NormalW);

	//----------------------------------
	// Displacement mapping
	//----------------------------------
	// Choose mipmap level based on distance to eye
	const float MipInterval = 20.0f;
	float mipLevel = clamp((distance(dOut.PosW, gEyePosW) - MipInterval) / MipInterval, 0.0f, 6.0f);

	// Sample height map (stored in alpha channel)
	float height = gNormalMap.SampleLevel(samLinear, dOut.Tex, mipLevel).a;

	// Offset vertex along normal
	dOut.PosW += (gHeightScale*(height-1.0))*dOut.NormalW;

	// Project to homogeneous clip space
	dOut.PosH = mul(float4(dOut.PosW, 1.0f), gViewProj);

	return dOut;
}

//============================================================================
// Pixel shader
//============================================================================
// Pixel shader is only used to cut out transparent pixels from shadow map
void PS(VertexOut pIn)
{
	float4 diffuse = gDiffuseMap.Sample(samLinear, pIn.Tex);

	// Dont write transparent pixels to shadow map
	clip(diffuse.a - 0.15f);
}

//============================================================================
// Pixel shader - Tessellation
//============================================================================
// Pixel shader is only used to cut out transparent pixels from shadow map
void TessPS(DomainOut pIn)
{
	float4 diffuse = gDiffuseMap.Sample(samLinear, pIn.Tex);

	// Dont write transparent pixels to shadow map
	clip(diffuse.a - 0.15f);
}

RasterizerState Depth
{
	// [From MSDN]
	// If the depth buffer currently bound to the output-merger stage has a UNORM format or
	// no depth buffer is bound the bias value is calculated like this: 
	//
	// Bias = (float)DepthBias * r + SlopeScaledDepthBias * MaxDepthSlope;
	//
	// where r is the minimum representable value > 0 in the depth-buffer format converted to float32.
	// [/End MSDN]
	// 
	// For a 24-bit depth buffer, r = 1 / 2^24.
	//
	// Example: DepthBias = 100000 ==> Actual DepthBias = 100000/2^24 = .006

	// Experimental values specific to different scenes
	DepthBias = 100000;
    DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
};

// Build shadow map without tessellation
technique11 BuildShadowMapTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(NULL);

		SetRasterizerState(Depth);
	}
};

// Build shadow map without tessellation, with alphaclip
technique11 BuildShadowMaphAlphaClipTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
};

// Build shadow map with tessellation
technique11 TessBuildShadowMapTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, TessVS()));
		SetHullShader(CompileShader(hs_5_0, HS()));
		SetDomainShader(CompileShader(ds_5_0, DS()));
		SetGeometryShader(NULL);
		SetPixelShader(NULL);

		SetRasterizerState(Depth);
	}
};

// Build shadow map with tessellation and alpha clip
technique11 TessBuildShadowMapAlphaClipTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, TessVS()));
		SetHullShader(CompileShader(hs_5_0, HS()));
		SetDomainShader(CompileShader(ds_5_0, DS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, TessPS()));
	}
};