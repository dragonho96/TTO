#pragma once
#include "Engine_Defines.h"
#include "Engine.h"

BEGIN(Engine)
class CShader;
class CTransform;
class CLine
{
public:
	explicit CLine();
	virtual ~CLine();

public:
	void Initialize(_float3* lines, UINT lineCount);
	void Render();

	CTransform* GetTransform() { return m_pTransform; }

protected:
	XMMATRIX world;
	_float3 position;
	_float3 rotation;
	_float3 scale;

	CTransform*		m_pTransform;

	Scope<CShader>	m_pShader;

	ComRef<ID3D11Buffer> vertexBuffer;
	UINT vertexCount, indexCount;

	SimpleVertex*					m_pVertices;
	D3D11_PRIMITIVE_TOPOLOGY		m_eTopology;
};

END