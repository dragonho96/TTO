#pragma once
#include "Client_Defines.h"
#include "Scene.h"

BEGIN(Client)
class CScene_GamePlay final : public CScene
{
private:
	explicit CScene_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelIndex);
	virtual ~CScene_GamePlay() = default;
public:
	virtual HRESULT Initialize() override;
	virtual _uint Update(_double TimeDelta)  override;
	virtual HRESULT Render() override;
public:
	static CScene_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelIndex);
	virtual void Free() override;
};

END
