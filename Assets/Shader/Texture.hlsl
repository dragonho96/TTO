cbuffer VS_ViewProjection : register(b0) // register buffer 0�� ���ڴٴ°�
{
    matrix _view;
    matrix _projection;
}

cbuffer VS_World : register(b1)
{
    matrix _world;
}


cbuffer PS_Color : register(b0) // ���ؽ� ���̴��� �����̹Ƿ� �ٽ� 0������
{
    float4 Color; 
}

struct VertexInput
{
    float4 position : POSITION0;
    float2 uv : UV0; // ��� �۾��� Semantic � �༮���� ����
    // 9�� �ǹ� �������־��µ� 10���� �ǹ� ���� ���� �����θ� �����ϸ� ��
};

// uv�� �ȼ� ���̴����� ó��
// ���ø� ����ϸ鼭 �������ֽǲ�

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : UV0;
};

SamplerState Sampler : register(s0); // sampler�� ���� �ƴ϶� s0
// �⺻�� �ᵵ �� �ȳ־����� �⺻�� ����
Texture2D Map : register(t0); // texture�� t0

PixelInput VS(VertexInput input)
{
    PixelInput output;
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = Map.Sample(Sampler, input.uv); // �����ϳ� �������ֽǲ�

    return color;
}