#pragma once
#include "Scene.h"
#include "PathFinding.h"

BEGIN(Client)

class CScene_Effect final : public CScene
{
private:
	explicit CScene_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iSceneIndex);
	virtual ~CScene_Effect() = default;
public:
	static CScene_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iSceneIndex);
	virtual void Free() override;

public:
	virtual HRESULT Initialize() override;
	virtual _uint Update(_double TimeDelta)  override;
	virtual HRESULT Render() override;
private:
	HRESULT ReadyLayerLights(string pLayerTag);
	HRESULT ReadyLayerCamera(string pLayerTag);
	HRESULT ReadyLayerEffect(string pLayerTag);
	HRESULT ReadyScript();

	CGameObject* m_pTrajectory = nullptr;
	CGameObject* m_pEffect = nullptr;
};

END