
#include "Shader_Defines.hpp"

cbuffer Matrices
{
    matrix g_WorldMatrix;
    matrix g_ViewMatrix;
    matrix g_ProjMatrix;
    
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

struct MeshBoneMatrices
{
    matrix BoneMatrices[512];
};

MeshBoneMatrices g_BoneMatrices;

Texture2D g_DiffuseTexture;

Texture2D depthMapTexture0;
Texture2D depthMapTexture1;

SamplerState g_DiffuseSampler
{
    AddressU = mirror;
    AddressV = mirror;
};
SamplerState g_SamplerClamp
{
    Filter = min_mag_mip_point;

    AddressU = clamp;
    AddressV = clamp;
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
    float4 lightViewPosition0 : TEXCOORD1;
    float3 lightPos0 : TEXCOORD2;
    float4 lightViewPosition1 : TEXCOORD3;
    float3 lightPos1 : TEXCOORD4;
    float4 vProjPos : TEXCOORD5;
};

struct VS_OUT_LIGHT_DEPTH
{
    float4 vPosition : SV_POSITION;
    float4 vLightDepthPosition : TEXCOORD0;
    float2 vTexUV : TEXCOORD1;
};

/* 정점의 스페이스 변환. (월드, 뷰, 투영행렬의 곱.)*/
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    //matrix matWV, matWVP;

    //matWV = mul(g_WorldMatrix, g_ViewMatrix);
    //matWVP = mul(matWV, g_ProjMatrix);

    //Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    //Out.vNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    //Out.vTexUV = In.vTexUV;
    //Out.vProjPos = Out.vPosition;


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
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexUV = In.vTexUV;
    Out.vProjPos = Out.vPosition;

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
    Out.vProjPos = Out.vPosition;

    //vector vWorldNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    //Out.fShade = saturate(dot(normalize(g_vLightDir) * -1.f, normalize(vWorldNormal)));

    return Out;
}


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

struct PS_IN_LIGHT_DEPTH
{
    float4 vPosition : SV_POSITION;
    float4 vLightDepthPosition : TEXCOORD0;
    float2 vTexUV : TEXCOORD1;
};
struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    // vector vLightDepth : SV_TARGET3;
};

struct PS_OUT_LIGHT_DEPTH
{
    vector vLightDepth : SV_TARGET0;
};

struct PS_OUT_BEHIND_WALL
{
    vector vDiffuse : SV_TARGET0;

};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

 //   Out.vDiffuse = g_DiffuseTexture.Sample(g_DiffuseSampler, In.vTexUV);

	///* In.vNormal.xyz : -1 ~ 1 */ 
	///* Out.vNormal.xyz : 0 ~ 1 */

 //   Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 0.f);

 //   // In.vProjPos.w = view space상의 z값
 //   // 0- 1사이 값만 저장 가능하기에 far로 나눠서 저장할 수 있게 만든다
 //   // In.vProjPos.z / In.vProjPos.w 는 결국 In.vPosition.z가 아닌가?
 //   Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);

    float bias;
    float4 color;
    float4 diffuseColor;
    float2 projectTexCoord;
    float depthValue;
    float lightDepthValue;
    float lightIntensity;
    float4 textureColor;
    float3 lightDir;

    bias = 0.0001f;

    color = vector(0.1f, 0.1f, 0.1f, 1.f);
    diffuseColor = vector(0.1f, 0.1f, 0.1f, 1.f);
    lightDir = -(-1.f, -1.f, -1.f);


    projectTexCoord.x = In.lightViewPosition0.x / In.lightViewPosition0.w / 2.f + 0.5f;
    projectTexCoord.y = -In.lightViewPosition0.y / In.lightViewPosition0.w / 2.f + 0.5f;

    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        depthValue = depthMapTexture0.Sample(g_SamplerClamp, projectTexCoord).x;

        lightDepthValue = In.lightViewPosition0.z / In.lightViewPosition0.w;
        lightDepthValue = lightDepthValue - bias;

        if (lightDepthValue < depthValue)
        {
            // 이 픽셀의 빛의 양을 계산합니다.
            //lightIntensity = saturate(dot(In.vNormal, In.lightPos0));
            lightIntensity = saturate(dot(In.vNormal, lightDir));
 
            if (lightIntensity > 0.0f)
            {
                // 확산 색과 광 강도의 양에 따라 최종 확산 색을 결정합니다.
                color += (diffuseColor * lightIntensity);
 
                // 최종 빛의 색상을 채웁니다.
                color - saturate(color);
            }
        }

    }
    else
    {
        lightIntensity = saturate(dot(In.vNormal, lightDir));
        if (lightIntensity > 0.0f)
        {
                // 확산 색과 광 강도의 양에 따라 최종 확산 색을 결정합니다.
            color += (diffuseColor * lightIntensity);
            color - saturate(color);
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
            // 이 픽셀의 빛의 양을 계산합니다.
            lightIntensity = saturate(dot(In.vNormal, In.lightPos1));
 
            if (lightIntensity > 0.0f)
            {
                // 확산 색과 광 강도의 양에 따라 최종 확산 색을 결정합니다.
                color += (diffuseColor * lightIntensity);
            }
        }

    }


    color = saturate(color);
        // 이 텍스처 좌표 위치에서 샘플러를 사용하여 텍스처에서 픽셀 색상을 샘플링합니다.
    textureColor = g_DiffuseTexture.Sample(g_DiffuseSampler, In.vTexUV);
 
    // 빛과 텍스처 색상을 결합합니다.
    color = color * textureColor;
    Out.vDiffuse = color;


    Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.w / 300.f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
    return Out;
}

PS_OUT_LIGHT_DEPTH PS_MAIN_LIGHT_DEPTH(PS_IN_LIGHT_DEPTH In)
{
    PS_OUT_LIGHT_DEPTH Out = (PS_OUT_LIGHT_DEPTH) 0;

    //float2 uvTex;
    //uvTex.x = In.vPosition.x / 1920.f;
    //uvTex.y = In.vPosition.y / 1080.f;

    //// float prevDepth = (depthMapTexture.Sample(g_DiffuseSampler, In.vTexUV)).x;
    //float2 projectTexCoord;
    //projectTexCoord.x = In.vLightDepthPosition.x / In.vLightDepthPosition.w / 2.f + 0.5f;
    //projectTexCoord.y = -In.vLightDepthPosition.y / In.vLightDepthPosition.w / 2.f + 0.5f;

    //float prevDepth = (depthMapTexture.Sample(g_DiffuseSampler, uvTex)).x;

    //if (prevDepth != 0.f && prevDepth < In.vLightDepthPosition.z / In.vLightDepthPosition.w)
    //    discard;

    Out.vLightDepth = vector(In.vLightDepthPosition.z / In.vLightDepthPosition.w,0.f, 0.f, 0.f);

    return Out;
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