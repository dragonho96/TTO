/////////////
// GLOBALS //
/////////////
cbuffer VS_ViewProjection : register(b0) // register buffer 0번 쓰겠다는거
{
	matrix _view;
	matrix _projection;
}

cbuffer VS_World : register(b1)
{
	matrix _world;
}

cbuffer PS_LightBuffer : register(b0)
{
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
	float4 position : POSITION;
    float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
};
/////////////
// GLOBALS //
/////////////
SamplerState SampleType  : register(s0);
Texture2D shaderTexture : register(t0);
////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType VS(VertexInputType input)
{
	PixelInputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, _world);
	output.position = mul(output.position, _view);
	output.position = mul(output.position, _projection);

	// Store the texture coordinates for the pixel shader.
    output.uv = input.uv;
	output.normal = mul(input.normal, (float3x3)_world);
	output.normal = normalize(output.normal);

	return output;
}

float4 PS(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.uv);
	lightDir = -lightDirection;
	lightIntensity = saturate(dot(input.normal, lightDir));
	color = saturate(diffuseColor * lightIntensity);
	color = color * textureColor;

	return color;
}