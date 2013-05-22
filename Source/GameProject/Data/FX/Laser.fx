//-----------------------------------------------------------------------
// Laser effect using the Geometry buffer to stream out particles
//-----------------------------------------------------------------------

cbuffer cbPerFrame
{
	float3 gEyePosW;
	
	// For when the emit position/direction is varying
	float3 gEmitPosW;
	float3 gEmitDirW;
	
	float gGameTime;
	float gTimeStep;
	float4x4 gViewProj; 

	// Fogging
	float gFogStart;
	float gFogRange;
	float4 gFogColor;
};

cbuffer cbFixed
{
	// Net constant acceleration used to accerlate the particles.
	float3 gAccelW = {0.0f, 0.0f, 0.0f};
	
	// Texture coordinates used to stretch texture over quad 
	// when we expand point particle into a quad.
	float2 gQuadTexC[4] = 
	{
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f)
	};
};

// Array of textures for texturing the particles.
Texture2DArray gTexArray;

// Random texture used to generate random numbers in shaders
Texture1D gRandomTex;

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

DepthStencilState NoDepthWrites
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
};

BlendState AdditiveBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

float3 RandUnitVec3(float offset)
{
	// Use game time plus offset to sample random texture.
	float u = (gGameTime + offset);
	
	// coordinates in [-1,1]
	float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;
	
	// project onto unit sphere
	return normalize(v);
}

//===========================================================================
// Stream-out tech
//===========================================================================
#define PT_EMITTER 0
#define PT_FLARE 1

struct Particle
{
    float3 InitialPosW : POSITION;
	float3 InitialVelW : VELOCITY;
	float2 SizeW : SIZE;
	float Age : AGE;
	uint Type : TYPE;
};

Particle StreamOutVS(Particle vIn)
{
	return vIn;
}

// Stream-out GS emits new particles and destroys old
[maxvertexcount(2)]
void StreamOutGS(point Particle gIn[1], inout PointStream<Particle> ptStream)
{
	gIn[0].Age += gTimeStep;

	if (gIn[0].Type == PT_EMITTER)
	{
		// If over certain age, emit new particle
		if (gIn[0].Age > 0.001f)
		{
			//float3 vRandom = RandUnitVec3(0.0f);
			//vRandom.x *= 0.5f;
			//vRandom.z *= 0.5f;

			Particle p;
			p.InitialPosW = gEmitPosW.xyz;
			p.InitialVelW = gEmitDirW * 1000.0f;
			p.SizeW = float2(2.0f, 2.0f);
			p.Age = 0.0f;
			p.Type = PT_FLARE;

			ptStream.Append(p);

			// Reset time to emit
			gIn[0].Age = 0.0f;
		}

		// Always keep emitters
		ptStream.Append(gIn[0]);
	}
	else
	{
		if (gIn[0].Age <= 1.0f)
			ptStream.Append(gIn[0]);
	}
}

GeometryShader gsStreamOut = ConstructGSWithSO(
	CompileShader(gs_5_0, StreamOutGS()),
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x");

technique11 StreamOutTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, StreamOutVS()));
		SetGeometryShader(gsStreamOut);

		// Disable Pixel shader & Depth buffer for stream-out only
		SetPixelShader(NULL);
        SetDepthStencilState(DisableDepth, 0);
	}
}

//===========================================================================
// Draw Tech
//===========================================================================

struct VertexOut
{
	float3 PosW : POSITION;
	float2 SizeW : SIZE;
	float4 Color : COLOR;
	uint Type : TYPE;
};

VertexOut DrawVS(Particle vIn)
{
	VertexOut vOut;
	float t = vIn.Age;

    // Constant acceleration equation
	vOut.PosW = 0.5f*t*t*(vIn.InitialVelW) + t*vIn.InitialVelW + vIn.InitialPosW;

	// fade color with time
	float opacity = 1.0f - smoothstep(0.0f, 1.0f, t/1.0f);
	vOut.Color = float4(t*5.0f, t*0.5f, t*0.5f, opacity);
	//vOut.Color.y = 0.0f;
	//vOut.Color.z = 0.0f;

	vOut.SizeW = vIn.SizeW;
	vOut.Type  = vIn.Type;
	
	return vOut;
}

struct GeoOut
{
	float4 PosH : SV_Position;
	float4 Color : COLOR;
	float2 Tex : TEXCOORD;
};

// Draw GS expands point into camera facing quads
[maxvertexcount(4)]
void DrawGS(point VertexOut gIn[1], inout TriangleStream<GeoOut> triStream)
{	
	// Do not draw emitter particles
	if( gIn[0].Type != PT_EMITTER )
	{
		// Compute world matrix so that billboard faces the camera.
		float3 look = normalize(gEyePosW.xyz - gIn[0].PosW);
		float3 right = normalize(cross(float3(0,1,0), look));
		float3 up = cross(look, right);

		// Compute triangle strip vertices (quad) in world space.
		float halfWidth  = 0.5f*gIn[0].SizeW.x;
		float halfHeight = 0.5f*gIn[0].SizeW.y;

		float4 v[4];
		v[0] = float4(gIn[0].PosW + halfWidth*right - halfHeight*up, 1.0f);
		v[1] = float4(gIn[0].PosW + halfWidth*right + halfHeight*up, 1.0f);
		v[2] = float4(gIn[0].PosW - halfWidth*right - halfHeight*up, 1.0f);
		v[3] = float4(gIn[0].PosW - halfWidth*right + halfHeight*up, 1.0f);

		// Transform quad vertices to world space and output 
		// them as a triangle strip.
		GeoOut gOut;
		[unroll]
		for(int i = 0; i < 4; ++i)
		{
			gOut.PosH = mul(v[i], gViewProj);
			gOut.Tex = gQuadTexC[i];
			gOut.Color = gIn[0].Color;
			triStream.Append(gOut);
		}	
	}
}

float4 DrawPS(GeoOut pIn) : SV_TARGET
{
	return gTexArray.Sample(samLinear, float3(pIn.Tex, 0))*pIn.Color;
}

technique11 DrawTech
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, DrawVS()));
        SetGeometryShader(CompileShader(gs_5_0, DrawGS()));
        SetPixelShader(CompileShader(ps_5_0, DrawPS()));
        
        SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetDepthStencilState(NoDepthWrites, 0);
    }
}