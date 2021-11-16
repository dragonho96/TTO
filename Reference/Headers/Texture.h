#pragma once
#include "Engine_Defines.h"
#include "Shader.h"
#include "ShaderBuffer.h"
#include "GlobalBuffers.h"

BEGIN(Engine)
class CTexture
{
private:
	class ColorBuffer; 

public:
	CTexture();
	~CTexture();

public:
	void Set();
	void Render();

private:
	CShader* shader;
	
	WorldBuffer*	 worldBuffer;
	ColorBuffer*	colorBuffer;
	ID3D11Buffer * vertexBuffer;
	ID3D11Buffer * indexBuffer;

	UINT vertexCount, indexCount;

	VertexTexture * vertices;
	UINT * indices;

	//ColorBuffer * colorBuffer;

	ID3D11ShaderResourceView * srv;

private:
	class ColorBuffer : public ShaderBuffer
	{
	public:
		ColorBuffer()
			: ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Color = _float4(1, 1, 1, 1);
		}

		~ColorBuffer()
		{

		}

		struct Struct
		{
			_float4 Color;
		} Data;
	};
};

END