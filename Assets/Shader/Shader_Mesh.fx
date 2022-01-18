
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
    matrix g_LightViewMatrix2;
    matrix g_LightProjMatrix2;
    matrix g_LightViewMatrix3;
    matrix g_LightProjMatrix3;
    matrix g_LightViewMatrix4;
    matrix g_LightProjMatrix4;
    matrix g_LightViewMatrix5;
    matrix g_LightProjMatrix5;
}

cbuffer LightBuffer
{
    float3 lightPosition0;
    float3 lightPosition1;
    float3 lightPosition2;
    float3 lightPosition3;
    float3 lightPosition4;
    float3 lightPosition5;

    float3 lightDir0;
    float3 lightDir1;
    float3 lightDir2;
    float3 lightDir3;
    float3 lightDir4;
    float3 lightDir5;

    float lightAngle0;
    float lightAngle1;
    float lightAngle2;
    float lightAngle3;
    float lightAngle4;
    float lightAngle5;
};

cbuffer Dissolve
{
    float g_fDissolve;
};
struct MeshBoneMatrices
{
    matrix BoneMatrices[512];
};

MeshBoneMatrices g_BoneMatrices;

Texture2D g_DiffuseTexture;
Texture2D g_DissolveTexture;
Texture2D g_DepthTexture;
Texture2D depthMapTexture0;
Texture2D depthMapTexture1;
Texture2D depthMapTexture2;
Texture2D depthMapTexture3;
Texture2D depthMapTexture4;
Texture2D depthMapTexture5;

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
    float4 lightViewPosition2 : TEXCOORD5;
    float3 lightPos2 : TEXCOORD6;
    float4 lightViewPosition3 : TEXCOORD7;
    float3 lightPos3 : TEXCOORD8;
    float4 lightViewPosition4 : TEXCOORD9;
    float3 lightPos4 : TEXCOORD10;
    float4 lightViewPosition5 : TEXCOORD11;
    float3 lightPos5 : TEXCOORD12;



    float4 vProjPos : TEXCOORD13;
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
    Out.vTexUV = In.vTexUV;
    Out.vNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    Out.vNormal = normalize(Out.vNormal);

    Out.lightViewPosition0 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition0 = mul(Out.lightViewPosition0, g_LightViewMatrix0);
    Out.lightViewPosition0 = mul(Out.lightViewPosition0, g_LightProjMatrix0);

    Out.lightViewPosition1 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition1 = mul(Out.lightViewPosition1, g_LightViewMatrix1);
    Out.lightViewPosition1 = mul(Out.lightViewPosition1, g_LightProjMatrix1);

    Out.lightViewPosition2 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition2 = mul(Out.lightViewPosition2, g_LightViewMatrix2);
    Out.lightViewPosition2 = mul(Out.lightViewPosition2, g_LightProjMatrix2);

    Out.lightViewPosition3 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition3 = mul(Out.lightViewPosition3, g_LightViewMatrix3);
    Out.lightViewPosition3 = mul(Out.lightViewPosition3, g_LightProjMatrix3);

    Out.lightViewPosition4 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition4 = mul(Out.lightViewPosition4, g_LightViewMatrix4);
    Out.lightViewPosition4 = mul(Out.lightViewPosition4, g_LightProjMatrix4);

    Out.lightViewPosition5 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition5 = mul(Out.lightViewPosition5, g_LightViewMatrix5);
    Out.lightViewPosition5 = mul(Out.lightViewPosition5, g_LightProjMatrix5);

    float4 worldPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightPos0 = lightPosition0.xyz - worldPosition.xyz;
    Out.lightPos1 = lightPosition1.xyz - worldPosition.xyz;
    Out.lightPos2 = lightPosition2.xyz - worldPosition.xyz;
    Out.lightPos3 = lightPosition3.xyz - worldPosition.xyz;
    Out.lightPos4 = lightPosition4.xyz - worldPosition.xyz;
    Out.lightPos5 = lightPosition5.xyz - worldPosition.xyz;
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

VS_OUT_LIGHT_DEPTH VS_MAIN_LIGHT_DEPTH_ANIM(VS_IN In)
{
    VS_OUT_LIGHT_DEPTH Out = (VS_OUT_LIGHT_DEPTH) 0;

    matrix matLightWV, matLightWVP;

    matLightWV = mul(g_WorldMatrix, g_LightViewMatrix0);
    matLightWVP = mul(matLightWV, g_LightProjMatrix0);

    matrix BoneMatrix = g_BoneMatrices.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.w] * In.vBlendWeight.w;

    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    Out.vPosition = mul(vPosition, matLightWVP);
    Out.vLightDepthPosition = Out.vPosition;
    Out.vTexUV = In.vTexUV;
    return Out;
}


VS_OUT_LIGHT_DEPTH VS_MAIN_LIGHT_DEPTH_ANIM_RAGDOLL(VS_IN In)
{
    VS_OUT_LIGHT_DEPTH Out = (VS_OUT_LIGHT_DEPTH) 0;

    matrix BoneMatrix = g_BoneMatrices.BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.BoneMatrices[In.vBlendIndex.w] * In.vBlendWeight.w;

    matrix matLightWV, matLightWVP;

    matLightWV = mul(BoneMatrix, g_LightViewMatrix0);
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
    Out.vPosition = mul(vPosition, matWVP);

    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexUV = In.vTexUV;
    Out.vProjPos = Out.vPosition;


    Out.lightViewPosition0 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition0 = mul(Out.lightViewPosition0, g_LightViewMatrix0);
    Out.lightViewPosition0 = mul(Out.lightViewPosition0, g_LightProjMatrix0);

    Out.lightViewPosition1 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition1 = mul(Out.lightViewPosition1, g_LightViewMatrix1);
    Out.lightViewPosition1 = mul(Out.lightViewPosition1, g_LightProjMatrix1);

    Out.lightViewPosition2 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition2 = mul(Out.lightViewPosition2, g_LightViewMatrix2);
    Out.lightViewPosition2 = mul(Out.lightViewPosition2, g_LightProjMatrix2);

    Out.lightViewPosition3 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition3 = mul(Out.lightViewPosition3, g_LightViewMatrix3);
    Out.lightViewPosition3 = mul(Out.lightViewPosition3, g_LightProjMatrix3);

    Out.lightViewPosition4 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition4 = mul(Out.lightViewPosition4, g_LightViewMatrix4);
    Out.lightViewPosition4 = mul(Out.lightViewPosition4, g_LightProjMatrix4);

    Out.lightViewPosition5 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition5 = mul(Out.lightViewPosition5, g_LightViewMatrix5);
    Out.lightViewPosition5 = mul(Out.lightViewPosition5, g_LightProjMatrix5);
    Out.vTexUV = In.vTexUV;



    float4 worldPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightPos0 = lightPosition0.xyz - worldPosition.xyz;
    Out.lightPos1 = lightPosition1.xyz - worldPosition.xyz;
    Out.lightPos2 = lightPosition2.xyz - worldPosition.xyz;
    Out.lightPos3 = lightPosition3.xyz - worldPosition.xyz;
    Out.lightPos4 = lightPosition4.xyz - worldPosition.xyz;
    Out.lightPos5 = lightPosition5.xyz - worldPosition.xyz;

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
    Out.vNormal = mul(vNormal, g_WorldMatrix);
    Out.vTexUV = In.vTexUV;
    Out.vProjPos = Out.vPosition;


    
    Out.lightViewPosition0 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition0 = mul(Out.lightViewPosition0, g_LightViewMatrix0);
    Out.lightViewPosition0 = mul(Out.lightViewPosition0, g_LightProjMatrix0);

    Out.lightViewPosition1 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition1 = mul(Out.lightViewPosition1, g_LightViewMatrix1);
    Out.lightViewPosition1 = mul(Out.lightViewPosition1, g_LightProjMatrix1);

    Out.lightViewPosition2 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition2 = mul(Out.lightViewPosition2, g_LightViewMatrix2);
    Out.lightViewPosition2 = mul(Out.lightViewPosition2, g_LightProjMatrix2);

    Out.lightViewPosition3 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition3 = mul(Out.lightViewPosition3, g_LightViewMatrix3);
    Out.lightViewPosition3 = mul(Out.lightViewPosition3, g_LightProjMatrix3);

    Out.lightViewPosition4 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition4 = mul(Out.lightViewPosition4, g_LightViewMatrix4);
    Out.lightViewPosition4 = mul(Out.lightViewPosition4, g_LightProjMatrix4);

    Out.lightViewPosition5 = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightViewPosition5 = mul(Out.lightViewPosition5, g_LightViewMatrix5);
    Out.lightViewPosition5 = mul(Out.lightViewPosition5, g_LightProjMatrix5);
    Out.vTexUV = In.vTexUV;



    float4 worldPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightPos0 = lightPosition0.xyz - worldPosition.xyz;
    Out.lightPos1 = lightPosition1.xyz - worldPosition.xyz;
    Out.lightPos2 = lightPosition2.xyz - worldPosition.xyz;
    Out.lightPos3 = lightPosition3.xyz - worldPosition.xyz;
    Out.lightPos4 = lightPosition4.xyz - worldPosition.xyz;
    Out.lightPos5 = lightPosition5.xyz - worldPosition.xyz;

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
    float4 lightViewPosition2 : TEXCOORD5;
    float3 lightPos2 : TEXCOORD6;
    float4 lightViewPosition3 : TEXCOORD7;
    float3 lightPos3 : TEXCOORD8;
    float4 lightViewPosition4 : TEXCOORD9;
    float3 lightPos4 : TEXCOORD10;
    float4 lightViewPosition5 : TEXCOORD11;
    float3 lightPos5 : TEXCOORD12;



    float4 vProjPos : TEXCOORD13;
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

    float bias;
    float4 color;
    float4 diffuseColor;
    float2 projectTexCoord;
    float depthValue;
    float lightDepthValue;
    float lightIntensity;
    float4 textureColor;
    float3 lightDir;

    bias = 0.00001f;

    color = vector(0.1f, 0.1f, 0.1f, 1.f);
    diffuseColor = vector(1.f, 1.f, 1.f, 1.f);
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
                color = saturate(color);
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
            color = saturate(color);
        }
    }


    diffuseColor = vector(1.f, 1.f, 1.f, 1.f);
    bias = 0.001f;

    projectTexCoord.x = In.lightViewPosition1.x / In.lightViewPosition1.w / 2.f + 0.5f;
    projectTexCoord.y = -In.lightViewPosition1.y / In.lightViewPosition1.w / 2.f + 0.5f;

    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        depthValue = depthMapTexture1.Sample(g_SamplerClamp, projectTexCoord).x;

        lightDepthValue = In.lightViewPosition1.z / In.lightViewPosition1.w;
        lightDepthValue = lightDepthValue - bias;

        float3 ogLightDir = float3(0.f, 0.f, 1.f);
        float3 pixDir = -normalize(In.lightPos1);

        if (lightDepthValue < depthValue)
        {
            float angle = dot(normalize(lightDir1), pixDir);
            float cutoff = cos(radians(lightAngle1));
            
            if (angle > cutoff)
            {
                lightIntensity = 1 - ((1 - angle) / (1 - cutoff));
                color += (diffuseColor * lightIntensity);
                color = saturate(color);
            }
        }
    }

    projectTexCoord.x = In.lightViewPosition2.x / In.lightViewPosition2.w / 2.f + 0.5f;
    projectTexCoord.y = -In.lightViewPosition2.y / In.lightViewPosition2.w / 2.f + 0.5f;

    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        depthValue = depthMapTexture2.Sample(g_SamplerClamp, projectTexCoord).x;

        lightDepthValue = In.lightViewPosition2.z / In.lightViewPosition2.w;
        lightDepthValue = lightDepthValue - bias;

        float3 ogLightDir = float3(0.f, 0.f, 1.f);
        float3 pixDir = -normalize(In.lightPos2);

        if (lightDepthValue < depthValue)
        {
            float angle = dot(normalize(lightDir2), pixDir);
            float cutoff = cos(radians(lightAngle2));
            
            if (angle > cutoff)
            {
                lightIntensity = 1 - ((1 - angle) / (1 - cutoff));
                color += (diffuseColor * lightIntensity);
                color = saturate(color);
            }
        }
    }

    projectTexCoord.x = In.lightViewPosition3.x / In.lightViewPosition3.w / 2.f + 0.5f;
    projectTexCoord.y = -In.lightViewPosition3.y / In.lightViewPosition3.w / 2.f + 0.5f;

    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        depthValue = depthMapTexture3.Sample(g_SamplerClamp, projectTexCoord).x;

        lightDepthValue = In.lightViewPosition3.z / In.lightViewPosition3.w;
        lightDepthValue = lightDepthValue - bias;

        float3 ogLightDir = float3(0.f, 0.f, 1.f);
        float3 pixDir = -normalize(In.lightPos3);

        if (lightDepthValue < depthValue)
        {
            float angle = dot(normalize(lightDir3), pixDir);
            float cutoff = cos(radians(lightAngle3));
            
            if (angle > cutoff)
            {
                lightIntensity = 1 - ((1 - angle) / (1 - cutoff));
                color += (diffuseColor * lightIntensity);
                color = saturate(color);
            }
        }
    }

    projectTexCoord.x = In.lightViewPosition4.x / In.lightViewPosition4.w / 2.f + 0.5f;
    projectTexCoord.y = -In.lightViewPosition4.y / In.lightViewPosition4.w / 2.f + 0.5f;

    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        depthValue = depthMapTexture4.Sample(g_SamplerClamp, projectTexCoord).x;

        lightDepthValue = In.lightViewPosition4.z / In.lightViewPosition4.w;
        lightDepthValue = lightDepthValue - bias;

        float3 ogLightDir = float3(0.f, 0.f, 1.f);
        float3 pixDir = -normalize(In.lightPos4);

        if (lightDepthValue < depthValue)
        {
            float angle = dot(normalize(lightDir4), pixDir);
            float cutoff = cos(radians(lightAngle4));
            
            if (angle > cutoff)
            {
                lightIntensity = 1 - ((1 - angle) / (1 - cutoff));
                color += (diffuseColor * lightIntensity);
                color = saturate(color);
            }
        }
    }

    projectTexCoord.x = In.lightViewPosition5.x / In.lightViewPosition5.w / 2.f + 0.5f;
    projectTexCoord.y = -In.lightViewPosition5.y / In.lightViewPosition5.w / 2.f + 0.5f;

    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        depthValue = depthMapTexture5.Sample(g_SamplerClamp, projectTexCoord).x;

        lightDepthValue = In.lightViewPosition5.z / In.lightViewPosition5.w;
        lightDepthValue = lightDepthValue - bias;

        float3 ogLightDir = float3(0.f, 0.f, 1.f);
        float3 pixDir = -normalize(In.lightPos5);

        if (lightDepthValue < depthValue)
        {
            float angle = dot(normalize(lightDir5), pixDir);
            float cutoff = cos(radians(lightAngle5));
            
            if (angle > cutoff)
            {
                lightIntensity = 1 - ((1 - angle) / (1 - cutoff));
                color += (diffuseColor * lightIntensity);
                color = saturate(color);
            }
        }
    }


    color = saturate(color);
        // 이 텍스처 좌표 위치에서 샘플러를 사용하여 텍스처에서 픽셀 색상을 샘플링합니다.
    textureColor = g_DiffuseTexture.Sample(g_DiffuseSampler, In.vTexUV);
    float4 dissolveColor = g_DissolveTexture.Sample(g_DiffuseSampler, In.vTexUV);
    // 빛과 텍스처 색상을 결합합니다.
    color = color * textureColor;

    if (dissolveColor.r < g_fDissolve)
    {
        if (dissolveColor.r > g_fDissolve - 0.2f && g_fDissolve != 1.f)    // Edge부분 체크
            color = float4(1.f, 0.f, 0.f, 0.8f);
        else
            color.a = 0.0f;     // 확실히 없어질 곳
    }


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
    float4 textureColor = g_DiffuseTexture.Sample(g_DiffuseSampler, In.vTexUV);
    if (textureColor.a < 0.8f || g_fDissolve > 0.1f)
        Out.vLightDepth = vector(1.f, 0.f, 0.f, 0.f);
    else
        Out.vLightDepth = vector(In.vLightDepthPosition.z / In.vLightDepthPosition.w, 0.f, 0.f, 0.f);
    return Out;
}
PS_OUT_BEHIND_WALL PS_MAIN_WALL(PS_IN In)
{
    PS_OUT_BEHIND_WALL Out = (PS_OUT_BEHIND_WALL) 0;

    float2 projectTexCoord;
    projectTexCoord.x = In.vProjPos.x / In.vProjPos.w / 2.f + 0.5f;
    projectTexCoord.y = -In.vProjPos.y / In.vProjPos.w / 2.f + 0.5f;

    float depthValue = g_DepthTexture.Sample(g_SamplerClamp, projectTexCoord).y;
    float camDapthValue = In.vProjPos.z / In.vProjPos.w;

    if (camDapthValue > depthValue + 0.0012f)
        Out.vDiffuse = vector(1.f, 0.5f, 0.f, 1.f);
    else
        discard;

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
        SetBlendState(Blend_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_ANIM();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass RagdollAnimation
    {
        SetRasterizerState(Rasterizer_Solid);
        SetDepthStencilState(DepthStecil_Default, 0);
        SetBlendState(Blend_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_ANIM_RAGDOLL();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass BehindWall
    {
        SetRasterizerState(Rasterizer_Solid);
        SetDepthStencilState(DepthStecil_NotZTest, 0);
        SetBlendState(Blend_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

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

    pass LightDepthAnim
    {
        SetRasterizerState(Rasterizer_Solid);
        SetDepthStencilState(DepthStecil_Default, 0);
        SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_LIGHT_DEPTH_ANIM();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DEPTH();
    }

    pass LightDepthRagdoll
    {
        SetRasterizerState(Rasterizer_Solid);
        SetDepthStencilState(DepthStecil_Default, 0);
        SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_LIGHT_DEPTH_ANIM_RAGDOLL();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DEPTH();
    }


}