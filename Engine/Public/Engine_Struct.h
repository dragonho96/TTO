#pragma once

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};


struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

//struct Vertex
//{
//	Vertex()
//		: Position(0.f, 0.f, 0.f) {}
//
//	_float3 Position;
//};