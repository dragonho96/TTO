#include "..\Public\RectTransform.h"
#include "Engine.h"


CRectTransform::CRectTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
	, m_pEngine(CEngine::GetInstance())
{
}

CRectTransform::CRectTransform(const CRectTransform & rhs)
	: CComponent(rhs)
	, m_TransformDesc(rhs.m_TransformDesc)
	, m_pEngine(CEngine::GetInstance())
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

bool CRectTransform::IsMouseInRect()
{
	int curWindowSizeX = (int)m_pEngine->GetCurrentWindowSize().x;
	int curWindowSizeY = (int)m_pEngine->GetCurrentWindowSize().y;
	float defaultWindowSizeX = 1280.f;
	float defaultWindowSizeY = 720.f;

	_float fSizeX = m_TransformDesc.sizeX * (curWindowSizeX / defaultWindowSizeX);
	_float fSizeY = m_TransformDesc.sizeY * (curWindowSizeY / defaultWindowSizeY);
	_float fPosX = m_TransformDesc.posX * (curWindowSizeX / defaultWindowSizeX);
	_float fPosY = m_TransformDesc.posY * (curWindowSizeY / defaultWindowSizeY);

	_float3 mousePos = m_pEngine->GetMousePosition();
	if (mousePos.x <= fPosX + (fSizeX / 2) &&
		mousePos.x >= fPosX - (fSizeX / 2) &&
		mousePos.y <= fPosY + (fSizeY / 2) &&
		mousePos.y >= fPosY - (fSizeY / 2))
	{
		return true;
	}
	return false;
}

void CRectTransform::SetTransformMat(RECTTRANSFORMDESC _desc)
{
	m_TransformDesc = _desc;

	int curWindowSizeX = (int)m_pEngine->GetCurrentWindowSize().x;
	int curWindowSizeY = (int)m_pEngine->GetCurrentWindowSize().y;

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

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((float)curWindowSizeX, (float)curWindowSizeY, 0.0f, 1.f));
}

void CRectTransform::SetTransformMat(_float4x4 _mat)
{
	int curWindowSizeX = (int)m_pEngine->GetCurrentWindowSize().x;
	int curWindowSizeY = (int)m_pEngine->GetCurrentWindowSize().y;
	float defaultWindowSizeX = 1280.f;
	float defaultWindowSizeY = 720.f;

	m_TransformMatrix = _mat;
	m_TransformDesc.sizeX = m_TransformMatrix._11 * (defaultWindowSizeX / curWindowSizeX);
	m_TransformDesc.sizeY = m_TransformMatrix._22 * (defaultWindowSizeY / curWindowSizeY);
	m_TransformDesc.posX = (m_TransformMatrix._41 + (curWindowSizeX >> 1)) * (defaultWindowSizeX / curWindowSizeX);
	m_TransformDesc.posY = (-m_TransformMatrix._42 + (curWindowSizeY >> 1)) * (defaultWindowSizeY / curWindowSizeY);


}

void CRectTransform::SetPosition(_float x, _float y)
{
	RECTTRANSFORMDESC newDesc = { x, y, m_TransformDesc.sizeX, m_TransformDesc.sizeY };
	SetTransformMat(newDesc);
}

void CRectTransform::SetClientPosition(_float x, _float y)
{
	int curWindowSizeX = (int)m_pEngine->GetCurrentWindowSize().x;
	int curWindowSizeY = (int)m_pEngine->GetCurrentWindowSize().y;
	float defaultWindowSizeX = 1280.f;
	float defaultWindowSizeY = 720.f;

	_float newX = x * (defaultWindowSizeX / curWindowSizeX);
	_float newY = y * (defaultWindowSizeY / curWindowSizeY);

	RECTTRANSFORMDESC newDesc = { newX, newY, m_TransformDesc.sizeX, m_TransformDesc.sizeY };
	SetTransformMat(newDesc);
}
