//--------------------------------------------------------------------------------------
// File: Tutorial05.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
    matrix g_WorldMatrix;
    matrix g_ViewMatrix;
    matrix g_ProjMatrix;
}

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float4 Color : COLOR;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul(input.Pos, g_WorldMatrix);
    output.Pos = mul(output.Pos, g_ViewMatrix);
    output.Pos = mul(output.Pos, g_ProjMatrix);
    output.Color = input.Color;
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
    return input.Color;
}


technique11 DefaultDevice
{
    pass DefaultPass
    {
        VertexShader = compile vs_5_0 VS();
        PixelShader = compile ps_5_0 PS();
    }
}