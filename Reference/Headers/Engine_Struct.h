#pragma once

namespace Engine
{
	typedef struct tagKeyFrameDesc
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vPosition;
		double			Time; 
	}KEYFRAMEDESC;

	typedef struct tagBoneDesc
	{
		char*		pName = nullptr;
		class CHierarchyNode*	pHierarchyNode = nullptr;
		// XMFLOAT4X4		OffsetMatrix * CombindMatrix(TransformationMatrix * pParent->CombindMatrix);
		XMFLOAT4X4		OffsetMatrix;
	}BONEDESC;

	typedef struct TagModelTextures
	{
		class CTexture*		pModelTexture[AI_TEXTURE_TYPE_MAX];
	}MODELTEXTURES;

	typedef struct tagVertex_Mesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndex;
		XMFLOAT4		vBlendWeight;
	}VTXMESH;

	typedef struct tagLightDesc
	{
		XMFLOAT4		vLightDir;

		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;
	}LIGHTDESC;

	typedef struct tagMaterialDesc
	{
		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;
	}MATERIALDESC;

	typedef struct tagVertex_Normal_TextureUV
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
	}VTXNORTEX;

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

	typedef struct tagEffectDesc
	{
		ID3DX11EffectPass*			pPass = nullptr;
		ID3D11InputLayout*			pLayout = nullptr;

		D3DX11_PASS_DESC			Desc;
		D3DX11_PASS_SHADER_DESC		PassVsDesc;
		D3DX11_EFFECT_SHADER_DESC	EffectVsDesc;
		vector<D3D11_SIGNATURE_PARAMETER_DESC> vecSignatureDescs;
	}EFFECTDESC;

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