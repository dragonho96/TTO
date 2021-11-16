#pragma once

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct VertexTexture
{
	VertexTexture()
		: Position(0, 0, 0)
		, Uv(0, 0)
		, Normal(0, 0, 0) {}

	XMFLOAT3	Position;
	XMFLOAT2	Uv;
	XMFLOAT3	Normal;
};

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

struct LightBufferType {
	XMFLOAT4 diffuseColor;
	XMFLOAT3 lightDirection;
	float padding; 
};

//struct Vertex
//{
//	Vertex()
//		: Position(0.f, 0.f, 0.f) {}
//
//	_float3 Position;
//};