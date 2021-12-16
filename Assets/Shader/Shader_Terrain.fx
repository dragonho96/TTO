
#include "Shader_Defines.hpp"

cbuffer Matrices
{
	matrix		g_WorldMatrix;
	matrix		g_ViewMatrix;
	matrix		g_ProjMatrix;	
}

cbuffer LightDesc
{
    vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
    vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
    vector g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
    vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);
}

cbuffer MaterialDesc
{
    vector g_vMtrlDiffuse;
    vector g_vMtrlAmbient = vector(0.1f, 0.1f, 0.1f, 1.f);
    vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
    float g_fPower = 30.f;
}

cbuffer EtcDesc
{
    vector g_vBrushPos = vector(10.f, 0.f, 10.f, 1.f);
    float g_fRange = 5.f;

    vector g_vCamPosition;
}

Texture2D g_DiffuseSourTexture;
Texture2D g_DiffuseDestTexture;
Texture2D g_BrushTexture;

Texture2D g_DiffuseTexture;
SamplerState g_DiffuseSampler
{
    Filter = min_mag_mip_linear;

    AddressU = wrap;
    AddressV = wrap;
};

Texture2D g_FilterTexture;
SamplerState g_FilterSampler
{
    Filter = min_mag_mip_point;

    AddressU = wrap;
    AddressV = wrap;
};

struct VS_IN
{
	float3 vPosition : POSITION; /* ���ý����̽� */
    float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4	vPosition : SV_POSITION;
    float fShade : COLOR0;
    float fSpecular : COLOR1;
	float2	vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

/* ������ �����̽� ��ȯ. (����, ��, ��������� ��.)*/
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexUV = In.vTexUV * 5.f;  // Tiling

    vector vWorldNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
	// Out.fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);
    Out.fShade = saturate(dot(normalize(g_vLightDir) * -1.f, normalize(vWorldNormal)));

    vector vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

    vector vLook = vWorldPos - g_vCamPosition;
    vector vReflect = reflect(normalize(g_vLightDir), normalize(vWorldNormal));

    Out.fSpecular = pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), g_fPower);

    Out.vWorldPos = vWorldPos;

    return Out;
}

/* VS_OUT ����ü�� SV_POSITION�̶�� �ø�ƽ�� ���� �����Ϳ� ���ؼ���  */
/* W������ XYZW��θ� ������. */
/* ����Ʈ�� ������ ��ġ�� ��ȯ�Ѵ�. */
/* �����Ͷ����� : �������� �ѷ��׿��� �ȼ��� ������ �����ϳ�.(�ѷ��׿��� ������ ������ ��������Ͽ�)  */

struct PS_IN
{
	float4	vPosition : SV_POSITION;
    float fShade : COLOR0;
    float fSpecular : COLOR1;
	float2	vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

vector	PS_MAIN(PS_IN In) : SV_TARGET
{
    vector vColor = (vector) 0;

    vector vSourDiffuse = g_DiffuseSourTexture.Sample(g_DiffuseSampler, In.vTexUV * 20.f);
    vector vDestDiffuse = g_DiffuseDestTexture.Sample(g_DiffuseSampler, In.vTexUV * 20.f);
	
	
    vector vFilter = g_FilterTexture.Sample(g_FilterSampler, In.vTexUV);

    vector vMtrlDiffuse = vSourDiffuse * vFilter.r + vDestDiffuse * (1.f - vFilter.r);

    if (g_vBrushPos.x - g_fRange < In.vWorldPos.x && In.vWorldPos.x < g_vBrushPos.x + g_fRange &&
		g_vBrushPos.z - g_fRange < In.vWorldPos.z && In.vWorldPos.z < g_vBrushPos.z + g_fRange)
    {
        float2 vTexUV = float2((In.vWorldPos.x - (g_vBrushPos.x - g_fRange)) / (2.f * g_fRange),
			((g_vBrushPos.z + g_fRange) - In.vWorldPos.z) / (2.f * g_fRange));

        vector vBrush = g_BrushTexture.Sample(g_DiffuseSampler, vTexUV);

        vMtrlDiffuse += vBrush;
    }

    vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(In.fShade + (g_vLightAmbient * g_vMtrlAmbient)) +
		(g_vLightSpecular * g_vMtrlSpecular) * In.fSpecular;
    vColor.a = vMtrlDiffuse.a;

    return vColor;
}

technique11		DefaultDevice
{
	pass DefaultPass
	{
        SetRasterizerState(Rasterizer_Solid);
        SetDepthStencilState(DepthStecil_Default, 0);
        SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}