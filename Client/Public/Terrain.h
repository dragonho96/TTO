#pragma once

#include "Engine.h"
#include "VIBuffer.h"
#include "Transform.h"

BEGIN(Client)
class CTerrain : public IScriptObject
{
private:
	explicit CTerrain();
	explicit CTerrain(CGameObject* pObj);
	virtual ~CTerrain() = default;
public:
	static CTerrain* Create(CGameObject* pObj);
	virtual void Free() override;

public:
	HRESULT Initialize();
	virtual void Update(_double deltaTime);
	virtual void LapteUpdate(_double deltaTime);
	virtual void Render();

private:
	CGameObject*	m_pGameObject;
	CVIBuffer*		m_pVIBuffer = nullptr;
	CShader*		m_pShader;
	CTransform*		m_pPlayerTransform = nullptr;

private:
	CTexture*		m_pTerrainTexture = nullptr;
	CTexture*		m_pFilterTexture = nullptr;
	CTexture*		m_pBrushTexture = nullptr;
};

END