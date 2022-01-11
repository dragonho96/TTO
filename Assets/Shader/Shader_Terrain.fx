
#include "Shader_Defines.hpp"

cbuffer Matrices
{
	matrix		g_WorldMatrix;
	matrix		g_ViewMatrix;
	matrix		g_ProjMatrix;	

    matrix g_LightViewMatrix0;
    matrix g_LightProjMatrix0;
    matrix g_LightViewMatrix1;
    matrix g_LightProjMatrix1;
}
cbuffer LightBuffer
{
    float3 lightPosition0;
    float3 lightPosition1;
};

cbuffer EtcDesc
{
    vector g_vBrushPos;
    float g_fRange = 5.f;

    vector g_vCamPosition;
}

Texture2D g_DiffuseSourTexture;
Texture2D g_DiffuseSourTextureNormal;
Texture2D g_DiffuseDestTexture;
Texture2D g_BrushTexture;

Texture2D g_DiffuseTexture;

Texture2D depthMapTexture0;
Texture2D depthMapTexture1;

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
SamplerState g_SamplerClamp
{
    Filter = min_mag_mip_point;

    AddressU = clamp;
    AddressV = clamp;
};

struct VS_IN
{
	float3 vPosition : POSITION; /* ���ý����̽� */
    float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 lightViewPosition0 : TEXCOORD1;
    float3 lightPos0 : TEXCOORD2;
    float4 lightViewPosition1 : TEXCOORD3;
    float3 lightPos1 : TEXCOORD4;
    float4 vProjPos : TEXCOORD5;
};

/* ������ �����̽� ��ȯ. (����, ��, ��������� ��.)*/
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    //matrix matWV, matWVP;

    //matWV = mul(g_WorldMatrix, g_ViewMatrix);
    //matWVP = mul(matWV, g_ProjMatrix);

    //Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    //Out.vTexUV = In.vTexUV * 5.f;  // Tiling
    //vector vWorldNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    //Out.vNormal = normalize(vWorldNormal.xyz);

    //vector vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);


    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vProjPos = Out.vPosition;

    Out.lightViewPosition0 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition0 = mul(Out.lightViewPosition0, g_LightViewMatrix0);
    Out.lightViewPosition0 = mul(Out.lightViewPosition0, g_LightProjMatrix0);

    Out.lightViewPosition1 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition1 = mul(Out.lightViewPosition1, g_LightViewMatrix1);
    Out.lightViewPosition1 = mul(Out.lightViewPosition1, g_LightProjMatrix1);

    Out.vTexUV = In.vTexUV;

    Out.vNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    Out.vNormal = normalize(Out.vNormal);

    float4 worldPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    
    Out.lightPos0 = normalize(lightPosition0.xyz - worldPosition.xyz);
    Out.lightPos1 = normalize(lightPosition1.xyz - worldPosition.xyz);


    return Out;
}

struct VS_OUT_LIGHT_DEPTH
{
    float4 vPosition : SV_POSITION;
    float4 vLightDepthPosition : TEXCOORD0;
    float2 vTexUV : TEXCOORD1;
};

VS_OUT_LIGHT_DEPTH VS_MAIN_LIGHT_DEPTH(VS_IN In)
{
    VS_OUT_LIGHT_DEPTH Out = (VS_OUT_LIGHT_DEPTH) 0;

    matrix matLightWV, matLightWVP;

    matLightWV = mul(g_WorldMatrix, g_LightViewMatrix0);
    matLightWVP = mul(matLightWV, g_LightProjMatrix0);
    Out.vPosition = mul(vector(In.vPosition, 1.f), matLightWVP);

    Out.vLightDepthPosition = Out.vPosition;

    Out.vTexUV = In.vTexUV;
    return Out;
}


/* VS_OUT ����ü�� SV_POSITION�̶�� �ø�ƽ�� ���� �����Ϳ� ���ؼ���  */
/* W������ XYZW��θ� ������. */
/* ����Ʈ�� ������ ��ġ�� ��ȯ�Ѵ�. */
/* �����Ͷ����� : �������� �ѷ��׿��� �ȼ��� ������ �����ϳ�.(�ѷ��׿��� ������ ������ ��������Ͽ�)  */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 lightViewPosition0 : TEXCOORD1;
    float3 lightPos0 : TEXCOORD2;
    float4 lightViewPosition1 : TEXCOORD3;
    float3 lightPos1 : TEXCOORD4;
    float4 vProjPos : TEXCOORD5;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

  //  vector vSourDiffuse = g_DiffuseSourTexture.Sample(g_DiffuseSampler, In.vTexUV * 20.f);
  //  vector vDestDiffuse = g_DiffuseDestTexture.Sample(g_DiffuseSampler, In.vTexUV * 20.f);
	
	
  //  vector vFilter = g_FilterTexture.Sample(g_FilterSampler, In.vTexUV);

  //  vector vMtrlDiffuse = vSourDiffuse * vFilter.r + vDestDiffuse * (1.f - vFilter.r);

  //  if (g_vBrushPos.x - g_fRange < In.vWorldPos.x && In.vWorldPos.x < g_vBrushPos.x + g_fRange &&
		//g_vBrushPos.z - g_fRange < In.vWorldPos.z && In.vWorldPos.z < g_vBrushPos.z + g_fRange)
  //  {
  //      float2 vTexUV = float2((In.vWorldPos.x - (g_vBrushPos.x - g_fRange)) / (2.f * g_fRange),
		//	((g_vBrushPos.z + g_fRange) - In.vWorldPos.z) / (2.f * g_fRange));

  //      vector vBrush = g_BrushTexture.Sample(g_DiffuseSampler, vTexUV);

  //      vMtrlDiffuse += vBrush;
  //  }

  //  Out.vDiffuse = vector(vMtrlDiffuse.rgb, 1.f) * 1.f;
  //  Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 0.f);
  //  Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
  // // Out.vLightDepth = vector(In.vLightDepthPosition.z / In.vLightDepthPosition.w, 0.f, 0.f, 0.f);



    float bias;
    float4 color;
    float4 diffuseColor;
    float2 projectTexCoord;
    float depthValue;
    float lightDepthValue;
    float lightIntensity;
    float4 textureColor;

    bias = 0.0001f;

    color = vector(0.1f, 0.1f, 0.1f, 1.f);
    diffuseColor = vector(0.1f, 0.1f, 0.1f, 1.f);



    projectTexCoord.x = In.lightViewPosition0.x / In.lightViewPosition0.w / 2.f + 0.5f;
    projectTexCoord.y = -In.lightViewPosition0.y / In.lightViewPosition0.w / 2.f + 0.5f;

    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        depthValue = depthMapTexture0.Sample(g_SamplerClamp, projectTexCoord).x;

        lightDepthValue = In.lightViewPosition0.z / In.lightViewPosition0.w;
        lightDepthValue = lightDepthValue - bias;

        if (lightDepthValue < depthValue)
        {
            // �� �ȼ��� ���� ���� ����մϴ�.
            lightIntensity = saturate(dot(In.vNormal, In.lightPos0));
 
            if (lightIntensity > 0.0f)
            {
                // Ȯ�� ���� �� ������ �翡 ���� ���� Ȯ�� ���� �����մϴ�.
                color += (diffuseColor * lightIntensity);
 
                // ���� ���� ������ ä��ϴ�.
            }
        }

    }

    projectTexCoord.x = In.lightViewPosition1.x / In.lightViewPosition1.w / 2.f + 0.5f;
    projectTexCoord.y = -In.lightViewPosition1.y / In.lightViewPosition1.w / 2.f + 0.5f;

    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        depthValue = depthMapTexture1.Sample(g_SamplerClamp, projectTexCoord).x;

        lightDepthValue = In.lightViewPosition1.z / In.lightViewPosition1.w;
        lightDepthValue = lightDepthValue - bias;

        if (lightDepthValue < depthValue)
        {
            // �� �ȼ��� ���� ���� ����մϴ�.
            lightIntensity = saturate(dot(In.vNormal, In.lightPos1));
 
            if (lightIntensity > 0.0f)
            {
                // Ȯ�� ���� �� ������ �翡 ���� ���� Ȯ�� ���� �����մϴ�.
                color += (diffuseColor * lightIntensity);
            }
        }

    }


    color = saturate(color);
        // �� �ؽ�ó ��ǥ ��ġ���� ���÷��� ����Ͽ� �ؽ�ó���� �ȼ� ������ ���ø��մϴ�.
    textureColor = g_DiffuseSourTexture.Sample(g_DiffuseSampler, In.vTexUV);
 
    // ���� �ؽ�ó ������ �����մϴ�.
    color = color * textureColor;
    Out.vDiffuse = color;


    Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
    return Out;
}

struct PS_IN_LIGHT_DEPTH
{
    float4 vPosition : SV_POSITION;
    float4 vLightDepthPosition : TEXCOORD0;
    float2 vTexUV : TEXCOORD1;
};
struct PS_OUT_LIGHT_DEPTH
{
    vector vLightDepth : SV_TARGET0;
};
PS_OUT_LIGHT_DEPTH PS_MAIN_LIGHT_DEPTH(PS_IN_LIGHT_DEPTH In)
{
    PS_OUT_LIGHT_DEPTH Out = (PS_OUT_LIGHT_DEPTH) 0;

    Out.vLightDepth = vector(In.vLightDepthPosition.z / In.vLightDepthPosition.w, 0.f, 0.f, 0.f);

    return Out;
}
//vector	PS_MAIN(PS_IN In) : SV_TARGET
//{
//    vector vNormal = g_DiffuseSourTextureNormal.Sample(g_DiffuseSampler, In.vTexUV * 10.f);
//    vector vWorldNormal = mul(vNormal, In.vWorldPos);
//    In.fShade = saturate(dot(normalize(g_vLightDir) * -1.f, normalize(vWorldNormal)));

//    vector vColor = (vector) 0;

//    vector vSourDiffuse = g_DiffuseSourTexture.Sample(g_DiffuseSampler, In.vTexUV * 10.f);
//    vector vDestDiffuse = g_DiffuseDestTexture.Sample(g_DiffuseSampler, In.vTexUV * 10.f);
	
	
//    vector vFilter = g_FilterTexture.Sample(g_FilterSampler, In.vTexUV);

//    vector vMtrlDiffuse = vSourDiffuse * vFilter.r + vDestDiffuse * (1.f - vFilter.r);

//    vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(In.fShade + (g_vLightAmbient * g_vMtrlAmbient)) +
//		(g_vLightSpecular * g_vMtrlSpecular) * In.fSpecular;
//    vColor.a = vMtrlDiffuse.a;

//    if (g_vBrushPos.x - g_fRange < In.vWorldPos.x && In.vWorldPos.x < g_vBrushPos.x + g_fRange &&
//		g_vBrushPos.z - g_fRange < In.vWorldPos.z && In.vWorldPos.z < g_vBrushPos.z + g_fRange)
//    {
//        float2 vTexUV = float2((In.vWorldPos.x - (g_vBrushPos.x - g_fRange)) / (2.f * g_fRange),
//			((g_vBrushPos.z + g_fRange) - In.vWorldPos.z) / (2.f * g_fRange));

//        vector vBrush = g_BrushTexture.Sample(g_DiffuseSampler, vTexUV);

//        vColor += vBrush;
//    }



//    return vColor;
//}

technique11		DefaultDevice
{
	pass DefaultPass
	{
        SetRasterizerState(Rasterizer_Solid);
        SetDepthStencilState(DepthStecil_Default, 0);
        SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass LightDepth
    {
        SetRasterizerState(Rasterizer_Solid);
        SetDepthStencilState(DepthStecil_Default, 0);
        SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_LIGHT_DEPTH();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DEPTH();
    }
}