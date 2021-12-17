#include "stdafx.h"
#include "..\Public\Terrain.h"

USING(Client)

CTerrain::CTerrain(CGameObject* pObj)
{
}

CTerrain* CTerrain::Create(CGameObject* pObj)
{
	CTerrain*		pInstance = new CTerrain(pObj);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create CTerrain");
		SafeRelease(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CTerrain::Free()
{
	SafeRelease(m_pTerrainTexture);
	SafeRelease(m_pFilterTexture);
	SafeRelease(m_pBrushTexture);
}

HRESULT CTerrain::Initialize()
{
	list<class CGameObject*> objList = CEngine::GetInstance()->GetGameObjectInLayer(0, "Terrain");
	if (objList.size() <= 0)
		return E_FAIL;

	m_pGameObject = objList.front();
	m_pVIBuffer = dynamic_cast<CVIBuffer*>(m_pGameObject->GetComponent("Com_VIBuffer"));
	m_pShader = m_pVIBuffer->GetShader();

	objList = CEngine::GetInstance()->GetGameObjectInLayer(0, "Player");
	if (objList.size() <= 0)
		return E_FAIL;

	CGameObject* player = objList.front();
	m_pPlayerTransform = dynamic_cast<CTransform*>(player->GetComponent("Com_Transform"));

	
	m_pTerrainTexture = CTexture::Create(
		CEngine::GetInstance()->GetDevice(), 
		CEngine::GetInstance()->GetDeviceContext(),
		CTexture::TYPE_TGA, "../../Assets/Texture/Grass_%d.tga", 2);

	m_pFilterTexture = CTexture::Create(
		CEngine::GetInstance()->GetDevice(),
		CEngine::GetInstance()->GetDeviceContext(),
		CTexture::TYPE_WIC, "../../Assets/Texture/Filter.bmp");

	m_pBrushTexture = CTexture::Create(
		CEngine::GetInstance()->GetDevice(),
		CEngine::GetInstance()->GetDeviceContext(),
		CTexture::TYPE_TGA, "../../Assets/Texture/Brush.tga");

	return S_OK;
}

void CTerrain::Update(_double deltaTime)
{
}

void CTerrain::LapteUpdate(_double deltaTime)
{
	m_pShader->SetUp_TextureOnShader("g_DiffuseSourTexture", m_pTerrainTexture, 0);
	m_pShader->SetUp_TextureOnShader("g_DiffuseDestTexture", m_pTerrainTexture, 1);
	m_pShader->SetUp_TextureOnShader("g_FilterTexture", m_pFilterTexture);
	m_pShader->SetUp_TextureOnShader("g_BrushTexture", m_pBrushTexture);

	_matrix		ViewMatrix = CEngine::GetInstance()->GetTransform(CPipeline::D3DTS_VIEW);
	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);
	m_pShader->SetUp_ValueOnShader("g_vCamPosition", &ViewMatrix.r[3], sizeof(_vector));

	_vector playerPos = m_pPlayerTransform->GetState(CTransform::STATE_POSITION);
	m_pShader->SetUp_ValueOnShader("g_vBrushPos", &playerPos, sizeof(_vector));
}

void CTerrain::Render()
{

}
