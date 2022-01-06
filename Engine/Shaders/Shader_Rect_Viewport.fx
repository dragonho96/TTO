


texture2D		g_DiffuseTexture;

SamplerState	g_DiffuseSampler
{
	AddressU = mirror;
	AddressV = mirror;
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

	vColor = g_DiffuseTexture.Sample(g_DiffuseSampler, In.vTexUV);

	return vColor;
}



technique11		DefaultDevice
{
	pass Viewport
	{
		VertexShader = compile vs_5_0 VS_MAIN_VIEWPORT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}