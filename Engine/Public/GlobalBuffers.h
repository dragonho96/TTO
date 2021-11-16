#pragma once
#include "Engine_Defines.h"
#include "ShaderBuffer.h"

BEGIN(Engine)
class WorldBuffer : public ShaderBuffer
{
public:
	WorldBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		Data.World = XMMatrixIdentity();
	}

	void SetMatrix(XMMATRIX mat)
	{
		Data.World = mat;
		XMMatrixTranspose(Data.World);
	}

	struct Struct
	{
		XMMATRIX World;
	};

private:
	Struct Data;
};

//class ViewProjectionBuffer : public ShaderBuffer
//{
//public:
//	ViewProjectionBuffer() : ShaderBuffer(&Data, sizeof(Struct))
//	{
//		Data.View = XMMatrixIdentity();
//		Data.Projection = XMMatrixIdentity();
//	}
//
//	void SetView(XMMATRIX mat)
//	{
//		Data.View = mat;
//		XMMatrixInverse(&Data.ViewInverse, NULL, &mat);
//
//		XMMATRIXTranspose(&Data.View, &Data.View);
//		XMMATRIXTranspose(&Data.ViewInverse, &Data.ViewInverse);
//	}
//
//	void SetProjection(XMMATRIX mat)
//	{
//		Data.Projection = mat;
//		XMMATRIXTranspose(&Data.Projection, &Data.Projection);
//	}
//
//	struct Struct
//	{
//		XMMATRIX View;
//		XMMATRIX Projection;
//		XMMATRIX ViewInverse;
//	};
//
//private:
//	Struct Data;
//};

//class LightBuffer : public ShaderBuffer
//{
//public:
//	LightBuffer() : ShaderBuffer(&Data, sizeof(Struct))
//	{
//		Data.Direction = D3DXVECTOR3(-1, -1, -1);
//		Data.Color = D3DXCOLOR(1, 1, 1, 1);
//
//		Data.Overcast = 1;
//		Data.Intensity = 0.5f;
//	}
//
//	struct Struct
//	{
//		D3DXVECTOR3 Direction;
//		float Padding;
//
//		D3DXCOLOR Color;
//		
//		float Overcast;
//		float Intensity;
//		float Padding2[2];
//	};
//
//	Struct Data;
//};

END