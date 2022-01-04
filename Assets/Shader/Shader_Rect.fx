#include "Shader_Defines.hpp"

cbuffer Matrices
{
	matrix		g_WorldMatrix;
	matrix		g_ViewMatrix;
	matrix		g_ProjMatrix;	
}

cbuffer Color
{
    float4 vColor;
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
	return vColor;
}

technique11		DefaultDevice
{
	pass DefaultPass
	{
        SetRasterizerState(Rasterizer_Solid);
        SetDepthStencilState(DepthStecil_NotZWrite, 0);
        SetBlendState(Blend_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

	//pass DefaultPass
	//{
	//	VertexShader = compile vs_5_0 VS_MAIN_A();
	//	PixelShader = compile ps_5_0 PS_MAIN_A();
	//}
}