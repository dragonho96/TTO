#pragma once
#include "Scene.h"
#include "PathFinding.h"

BEGIN(Client)

class CScene_Test final : public CScene
{
private:
	explicit CScene_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iSceneIndex);
	virtual ~CScene_Test() = default;
public:
	static CScene_Test* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iSceneIndex);
	virtual void Free() override;

public:
	virtual HRESULT Initialize() override;
	virtual _uint Update(_double TimeDelta)  override;
	virtual HRESULT Render() override;
private:
	HRESULT ReadyPrototypeGameObject(); // 바로 추가
	HRESULT ReadyLayerCamera(string pLayerTag); /* 복제본을 생성하여 레이어에 추가하는 형태 .*/
	HRESULT ReadyLayerGrid(string pLayerTag);
	HRESULT ReadyScript();

private:
	CPathFinding* m_pPathFinding;
};

END