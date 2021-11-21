
cbuffer Matrices
{
	matrix		g_WorldMatrix;
	matrix		g_ViewMatrix;
	matrix		g_ProjMatrix;	
}


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

/* 정점의 스페이스 변환. (월드, 뷰, 투영행렬의 곱.)*/
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

/* VS_OUT 구조체의 SV_POSITION이라는 시멘틱을 가진 데이터에 한해서만  */
/* W값으로 XYZW모두를 나눈다. */
/* 뷰포트로 정점의 위치를 변환한다. */
/* 래스터라이즈 : 정점으로 둘러쌓여진 픽셀의 정보를 생성하낟.(둘러쌓여진 정점의 정보를 기반으로하여)  */

struct PS_IN
{
	float4	vPosition : SV_POSITION;
	float2	vTexUV : TEXCOORD0;
};

vector	PS_MAIN(PS_IN In) : SV_TARGET
{
	vector		vColor = (vector)0;

	vColor = vector(1.f, 0.f, 0.f, 1.f);

	if (In.vTexUV.x < 0.01f || 
		In.vTexUV.x > 0.99f || 
		In.vTexUV.y < 0.01f || 
		In.vTexUV.y > 0.99f
		)
		vColor = vector(1.f, 1.f, 0.f, 1.f);

	return vColor;
}

technique11		DefaultDevice
{
	pass DefaultPass
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	//pass DefaultPass
	//{
	//	VertexShader = compile vs_5_0 VS_MAIN_A();
	//	PixelShader = compile ps_5_0 PS_MAIN_A();
	//}
}