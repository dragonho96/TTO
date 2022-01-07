

cbuffer	LightDesc
{
	vector		g_vLightDir;
};


Texture2D		g_NormalTexture;
Texture2D		g_DiffuseTexture;
Texture2D		g_ShadeTexture;

SamplerState	g_DefaultSampler
{
	AddressU = wrap;
	AddressV = wrap;
};

struct VS_IN
{
	float3 vPosition : POSITION; /* 로컬스페이스 */
	float2 vTexUV : TEXCOORD0;

};

struct VS_OUT
{
	float4	vPosition : SV_POSITION;
	float2	vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN_VIEWPORT(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	Out.vPosition = vector(In.vPosition, 1.f);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4	vPosition : SV_POSITION;
	float2	vTexUV : TEXCOORD0;
};

vector	PS_MAIN(PS_IN In) : SV_TARGET
{
	vector		vColor = (vector)0;

	vColor = g_DiffuseTexture.Sample(g_DefaultSampler, In.vTexUV);

	return vColor;
}

struct PS_OUT_LIGHTACC
{
	vector	vShade : SV_TARGET0;
};

PS_OUT_LIGHTACC PS_MAIN_DIRECTION(PS_IN In)
{
	PS_OUT_LIGHTACC		Out = (PS_OUT_LIGHTACC)1;

	vector		vNormalDesc = g_NormalTexture.Sample(g_DefaultSampler, In.vTexUV);

	/* 0 ~ 1 */
	/* -1 ~ 1 */
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	Out.vShade.xyz = max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f);
	

	return Out;
}

struct PS_OUT_BLEND
{
	vector	vColor : SV_TARGET0;
};

PS_OUT_BLEND PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT_BLEND		Out = (PS_OUT_BLEND)0;

	vector		vDiffuseColor = g_DiffuseTexture.Sample(g_DefaultSampler, In.vTexUV);
	vector		vShade = g_ShadeTexture.Sample(g_DefaultSampler, In.vTexUV);

	Out.vColor = vDiffuseColor * vShade;

	if (Out.vColor.a == 0.0f)
		discard;

	return Out;

}

BlendState Blend_None
{
	BlendEnable[0] = false;
};

DepthStencilState  DepthStecil_NotZTest
{
	DepthEnable = false;
	DepthWriteMask = all;
	DepthFunc = always;
};

RasterizerState Rasterizer_Solid
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

technique11		DefaultDevice
{
	pass Debug
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_NotZTest, 0);
		SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_VIEWPORT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Directional
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_NotZTest, 0);
		SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_VIEWPORT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTION();
	}

	pass Point
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_NotZTest, 0);
		SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_VIEWPORT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Blend
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_NotZTest, 0);
		SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_VIEWPORT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

}