#include "..\Public\RectTransform.h"



CRectTransform::CRectTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CRectTransform::CRectTransform(const CRectTransform & rhs)
	: CComponent(rhs)
	, m_TransformDesc(rhs.m_TransformDesc)
{
}

CRectTransform * CRectTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRectTransform*	pInstance = new CRectTransform(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed To Create CRectTransform");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CRectTransform::Clone(void * pArg)
{
	CComponent*	pInstance = new CRectTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone CRectTransform");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CRectTransform::Free()
{
	__super::Free();
}

HRESULT CRectTransform::InitializePrototype()
{
	ZeroMemory(&m_TransformDesc, sizeof(RECTTRANSFORMDESC));
	m_TransformDesc.sizeX = 50;
	m_TransformDesc.sizeY = 50;
	m_TransformDesc.posX = 50;
	m_TransformDesc.posY = 50;
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
	return S_OK;
}

HRESULT CRectTransform::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(RECTTRANSFORMDESC));

	SetTransformMat(m_TransformDesc);
	return S_OK;
}

void CRectTransform::SetTransformMat(RECTTRANSFORMDESC _desc)
{
	int curWindowSizeX = 1280;
	int curWindowSizeY = 720;

	float defaultWindowSizeX = 1280.f;
	float defaultWindowSizeY = 720.f;


	_float fSizeX = _desc.sizeX * (curWindowSizeX / defaultWindowSizeX);
	_float fSizeY = _desc.sizeY * (curWindowSizeY / defaultWindowSizeY);
	_float fPosX = _desc.posX * (curWindowSizeX / defaultWindowSizeX);
	_float fPosY = _desc.posY * (curWindowSizeY / defaultWindowSizeY);

	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixIdentity());

	m_TransformMatrix._11 = fSizeX;
	m_TransformMatrix._22 = fSizeY;

	m_TransformMatrix._41 = fPosX - (curWindowSizeX >> 1);
	m_TransformMatrix._42 = -fPosY + (curWindowSizeY >> 1);

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(curWindowSizeX, curWindowSizeY, 0.0f, 1.f));


}

void CRectTransform::SetTransformMat(_float4x4 _mat)
{
	int curWindowSizeX = 1280;
	int curWindowSizeY = 720;
	float defaultWindowSizeX = 1280.f;
	float defaultWindowSizeY = 720.f;

	m_TransformMatrix = _mat;
	m_TransformDesc.sizeX = m_TransformMatrix._11 * (defaultWindowSizeX / curWindowSizeX);
	m_TransformDesc.sizeY = m_TransformMatrix._22 * (defaultWindowSizeY / curWindowSizeY);
	m_TransformDesc.posX = (m_TransformMatrix._41 + (curWindowSizeX >> 1)) * (defaultWindowSizeX / curWindowSizeX);
	m_TransformDesc.posY = (-m_TransformMatrix._42 + (curWindowSizeY >> 1)) * (defaultWindowSizeY / curWindowSizeY);
}
