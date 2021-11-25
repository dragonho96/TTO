
cbuffer Matrices
{
	matrix		g_WorldMatrix;
	matrix		g_ViewMatrix;
	matrix		g_ProjMatrix;	
}


struct VS_IN
{
	float3 vPosition : POSITION; /* ���ý����̽� */
    float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4	vPosition : SV_POSITION;
	float2	vTexUV : TEXCOORD0;
};

/* ������ �����̽� ��ȯ. (����, ��, ��������� ��.)*/
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

/* VS_OUT ����ü�� SV_POSITION�̶�� �ø�ƽ�� ���� �����Ϳ� ���ؼ���  */
/* W������ XYZW��θ� ������. */
/* ����Ʈ�� ������ ��ġ�� ��ȯ�Ѵ�. */
/* �����Ͷ����� : �������� �ѷ��׿��� �ȼ��� ������ �����ϳ�.(�ѷ��׿��� ������ ������ ��������Ͽ�)  */

struct PS_IN
{
	float4	vPosition : SV_POSITION;
	float2	vTexUV : TEXCOORD0;
};

vector	PS_MAIN(PS_IN In) : SV_TARGET
{
	vector		vColor = (vector)0;

	//if (In.vTexUV.x < 0.01f || 
	//	In.vTexUV.x > 0.99f || 
	//	In.vTexUV.y < 0.01f || 
	//	In.vTexUV.y > 0.99f
	//	)
	//	vColor = vector(1.f, 1.f, 0.f, 1.f);

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