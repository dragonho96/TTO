#pragma once
#include "Scene.h"

BEGIN(Client)

class CScene_Lobby final : public CScene
{
private:
	explicit CScene_Lobby(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iSceneIndex);
	virtual ~CScene_Lobby() = default;
public:
	static CScene_Lobby* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iSceneIndex);
	virtual void Free() override;

public:
	virtual HRESULT Initialize() override;
	virtual _uint Update(_double TimeDelta)  override;
	virtual HRESULT Render() override;

private:
	HRESULT ReadyLayerCamera(string pLayerTag);
};

END