#pragma once

namespace Engine
{
	typedef struct tagVertex_TextureUV
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexUV;
	}VTXTEX;

	typedef struct tagVertex_Color
	{
		XMFLOAT3		vPos;
		XMFLOAT4		vColor;
	}VTXCOLOR;

	typedef struct tagPolygonIndices16
	{
		unsigned short	_0, _1, _2;
	}POLYGONINDICES16;

	typedef struct tagPolygonIndices32
	{
		unsigned long	_0, _1, _2;
	}POLYGONINDICES32;
}


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