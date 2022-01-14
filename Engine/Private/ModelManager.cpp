#include "..\Public\ModelManager.h"
#include "Engine.h"

IMPLEMENT_SINGLETON(CModelManager)

CModelManager::CModelManager()
{

}

void CModelManager::Free()
{
	// Delete prototypes
	for (auto& model : m_mapModel)
		SafeRelease(model.second);

	m_mapModel.clear();

	SafeRelease(m_pDissolveTex);
}

CComponent* CModelManager::CloneModel(string pMeshFilePath, string pMeshFileName, string pShaderFilePath, _bool meshCollider, void* pArg)
{
	if (nullptr == m_pDissolveTex)
	{
		m_pDissolveTex = CTexture::Create(CEngine::GetInstance()->GetDevice(),
			CEngine::GetInstance()->GetDeviceContext(),
			CTexture::TYPE_TGA,
			"..\\..\\Assets\\Textures\\Dissolve\\Dissolve.tga");
	}


	ID3D11Device * pDevice = CEngine::GetInstance()->GetDevice();
	ID3D11DeviceContext * pDeviceContext = CEngine::GetInstance()->GetDeviceContext();

	string fullPath = pMeshFilePath + pMeshFileName;

	if (m_mapModel.find(fullPath) == m_mapModel.end())
	{
		CModel* pModel = CModel::Create(pDevice, pDeviceContext);
		pModel->SetMeshCollider(meshCollider);
		pModel->CreateBuffer(pMeshFilePath, pMeshFileName, pShaderFilePath);
		m_mapModel.emplace(fullPath, pModel);
	}

	// return cloned object
	return m_mapModel[fullPath]->Clone(pArg);
}
