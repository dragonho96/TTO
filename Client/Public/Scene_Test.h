#pragma once
#include "Scene.h"

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
	HRESULT ReadyPrototypeGameObject(); // �ٷ� �߰�
	HRESULT ReadyLayerTerrain(const _tchar* pLayerTag); /* �������� �����Ͽ� ���̾ �߰��ϴ� ���� .*/

};

END