
#include "Shader_Defines.hpp"

cbuffer Matrices
{
    matrix g_WorldMatrix;
    matrix g_ViewMatrix;
    matrix g_ProjMatrix;
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
    vector g_vMtrlAmbient = vector(0.5f, 0.5f, 0.5f, 1.f);
    vector g_vMtrlSpecular = vector(0.1f, 0.1f, 0.1f, 1.f);
    float g_fPower = 30.f;
}
struct MeshBoneMatrices
{
    matrix BoneMatrices[512];
};

MeshBoneMatrices g_BoneMatrices;

Texture2D g_DiffuseTexture;

SamplerState g_DiffuseSampler
{
    AddressU = mirror;
    AddressV = mirror;
};

struct VS_IN
{
    float3 vPosition : POSITION; /* 로컬스페이스 */
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float3 vTangent : TANGENT;
    uint4 vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;

    //float4 vPosition : SV_POSITION;
    //float2 vTexUV : TEXCOORD0;
    //float fShade : COLOR0;
    //float fSpecular : COLOR1;
};

/* 정점의 스페이스 변환. (월드, 뷰, 투영행렬의 곱.)*/
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    Out.vTexUV = In.vTexUV;
    
    
    //vector vWorldNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    // Out.fShade = saturate(dot(normalize(g_vLightDir) * -1.f, normalize(vWorldNormal)));
    return Out;
}

VS_OUT VS_MAIN_ANIM(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    matrix BoneMatrix = g_BoneMatrices.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.w] * In.vBlendWeight.w;

   
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);


    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = mul(vNormal, g_WorldMatrix);
    Out.vTexUV = In.vTexUV;

    // vector vWorldNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    // Out.fShade = saturate(dot(normalize(g_vLightDir) * -1.f, normalize(vWorldNormal)));

    return Out;
}

VS_OUT VS_MAIN_ANIM_RAGDOLL(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;
    
    matrix BoneMatrix = g_BoneMatrices.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.w] * In.vBlendWeight.w;
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

    matWV = mul(BoneMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
   //  Out.vNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    Out.vNormal = mul(vNormal, g_WorldMatrix);
    Out.vTexUV = In.vTexUV;

    //vector vWorldNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    //Out.fShade = saturate(dot(normalize(g_vLightDir) * -1.f, normalize(vWorldNormal)));

    return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;

    //float4 vPosition : SV_POSITION;
    //float2 vTexUV : TEXCOORD0;
    //float fShade : COLOR0;
    //float fSpecular : COLOR1;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
};

struct PS_OUT_BEHIND_WALL
{
    vector vDiffuse : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vDiffuse = g_DiffuseTexture.Sample(g_DiffuseSampler, In.vTexUV);

	/* In.vNormal.xyz : -1 ~ 1 */ 
	/* Out.vNormal.xyz : 0 ~ 1 */

    Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 0.f);

    return Out;
  //  vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_DiffuseSampler, In.vTexUV);

  //  vector vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(In.fShade + (g_vLightAmbient * g_vMtrlAmbient)) +
		//(g_vLightSpecular * g_vMtrlSpecular) * In.fSpecular;
  //  vColor.a = vMtrlDiffuse.a;

  //  if (vColor.a <= 0)
  //      discard;
  //  return vColor;
}

PS_OUT_BEHIND_WALL PS_MAIN_WALL(PS_IN In)
{
    PS_OUT_BEHIND_WALL Out = (PS_OUT_BEHIND_WALL) 0;

    Out.vDiffuse = vector(1.f, 0.5f, 0.f, 1.f);
    return Out;
}


technique11 DefaultDevice
{
    pass NonAnimation
    {
        SetRasterizerState(Rasterizer_Solid);
        SetDepthStencilState(DepthStecil_Default, 0);
        SetBlendState(Blend_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass Animation
    {
        SetRasterizerState(Rasterizer_Solid);
        SetDepthStencilState(DepthStecil_Default, 0);
        SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_ANIM();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass RagdollAnimation
    {
        SetRasterizerState(Rasterizer_Solid);
        SetDepthStencilState(DepthStecil_Default, 0);
        SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_ANIM_RAGDOLL();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass BehindWall
    {
        SetRasterizerState(Rasterizer_Solid);
        SetDepthStencilState(DepthStecil_NotZTest, 0);
        SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_ANIM();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_WALL();
    }
}