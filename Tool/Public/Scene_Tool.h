#pragma once

BEGIN(Tool)
class CScene_Tool final : public CScene
{
private:
	explicit CScene_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelIndex);
	virtual ~CScene_Tool() = default;
public:
	virtual HRESULT Initialize() override;
	virtual _uint Update(_double TimeDelta)  override;
	virtual HRESULT Render() override;
private:
	HRESULT ReadyPrototypeComponent();
	HRESULT ReadyPrototypeGameObject(); /* 로고에서 사용할 객체의 원형을 생성한다. (다른 씬의 경ㅇ우, 로더에서 구현될 내용.) */

	HRESULT Ready_Layer_Camera(string pLayerTag);
public:
	static CScene_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelIndex);
	virtual void Free() override;

private:
	CRenderer*	m_pRenderer;
};

END