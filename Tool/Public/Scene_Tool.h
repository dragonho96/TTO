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
	HRESULT ReadyPrototypeGameObject(); /* �ΰ��� ����� ��ü�� ������ �����Ѵ�. (�ٸ� ���� �椷��, �δ����� ������ ����.) */

	HRESULT Ready_Layer_Camera(string pLayerTag);
public:
	static CScene_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelIndex);
	virtual void Free() override;

private:
	CRenderer*	m_pRenderer;
};

END