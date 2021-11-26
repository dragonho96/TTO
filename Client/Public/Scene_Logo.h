#pragma once
#include "Client_Defines.h"
#include "Scene.h"

BEGIN(Client)

class CScene_Logo final : public CScene
{
private:
	explicit CScene_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iSceneIndex);
	virtual ~CScene_Logo() = default;
public:
	virtual HRESULT Initialize() override;
	virtual _uint Update(_double TimeDelta)  override;
	virtual HRESULT Render() override;
private:
	HRESULT ReadyPrototypeGameObject(); /* 로고에서 사용할 객체의 원형을 생성한다. (다른 씬의 경ㅇ우, 로더에서 구현될 내용.) */
	HRESULT ReadyLayerBackGround(string pLayerTag); /* 복제본을 생성하여 레이어에 추가하는 형태 .*/
public:
	static CScene_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelIndex);
	virtual void Free() override;
};

END