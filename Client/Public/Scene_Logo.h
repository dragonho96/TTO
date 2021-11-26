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
	HRESULT ReadyPrototypeGameObject(); /* �ΰ��� ����� ��ü�� ������ �����Ѵ�. (�ٸ� ���� �椷��, �δ����� ������ ����.) */
	HRESULT ReadyLayerBackGround(string pLayerTag); /* �������� �����Ͽ� ���̾ �߰��ϴ� ���� .*/
public:
	static CScene_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelIndex);
	virtual void Free() override;
};

END