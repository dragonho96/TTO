#include "..\Public\ModelManager.h"
#include "Engine.h"

IMPLEMENT_SINGLETON(CModelManager)
static int iCurNumThread = 0;

unsigned int APIENTRY ThreadCloneModel(void* pArg)
{
	CModelManager::MODELLOADDESC* desc = (CModelManager::MODELLOADDESC*)pArg;

	EnterCriticalSection(&desc->cs);

	++iCurNumThread;

	CModelManager::GetInstance()->CloneModelThread(
		desc->pObj, desc->pMeshFilePath, desc->pMeshFileName, desc->pShaderFilePath,
		desc->bMeshCollider, desc->pArg);

	LeaveCriticalSection(&desc->cs);

	SafeDelete(desc);
	--iCurNumThread;


	return 0;
}

CModelManager::CModelManager()
{
	InitializeCriticalSection(&m_CS);
}

void CModelManager::Free()
{
	// Delete prototypes
	for (auto& model : m_mapModel)
		SafeRelease(model.second);

	m_mapModel.clear();

	SafeRelease(m_pDissolveTex);

	DeleteCriticalSection(&m_CS);

}

void CModelManager::CloneModel(CGameObject* pObj, string pMeshFilePath, string pMeshFileName, string pShaderFilePath, _bool meshCollider, void* pArg)
{
	while (10 < iCurNumThread)
		Sleep(100);
	
	string fullPath = pMeshFilePath + pMeshFileName;
	auto& iter = m_CurCloningObj.find(fullPath);
	if (iter != m_CurCloningObj.end())
	{
		while(iter->second == true)
			Sleep(100);
	}
	else
		m_CurCloningObj.emplace(fullPath, true);

	//if (m_CurCloningObj.find(fullPath) != m_CurCloningObj.end())
	//{
	//	while (iter->second != true)
	//		Sleep(1000);
	//}
	//else
	//{
	//	m_CurCloningObj.emplace(fullPath, true);
	//}

	MODELLOADDESC* desc = new MODELLOADDESC(pObj, pMeshFilePath, pMeshFileName, pShaderFilePath, meshCollider, pArg, m_CS);
	thread_handles.emplace((HANDLE)_beginthreadex(nullptr, 0, ThreadCloneModel, desc, 0, nullptr));
}

void CModelManager::CloneModelThread(CGameObject* pObj, string pMeshFilePath, string pMeshFileName, string pShaderFilePath, _bool meshCollider, void * pArg)
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

		auto& iter = m_CurCloningObj.find(fullPath);
		if (iter != m_CurCloningObj.end())
			iter->second = false;
	}

	// return cloned object
	pObj->AddModelComponent(0, m_mapModel[fullPath]->Clone(pArg));
}

void CModelManager::WaitThreads()
{
	while (!thread_handles.empty())
	{
		set<HANDLE> threads_left;
		for (set<HANDLE>::iterator cur_thread = thread_handles.begin(), last = thread_handles.end();
			cur_thread != last; ++cur_thread)
		{
			DWORD rc = WaitForSingleObject(*cur_thread, INFINITE);
			if (rc == WAIT_OBJECT_0)
			{
				CloseHandle(*cur_thread);

			}
			else if (rc == WAIT_TIMEOUT)
			{
				threads_left.emplace(*cur_thread);
			}
			else
			{
				CloseHandle(*cur_thread);
			}
		}
		swap(threads_left, thread_handles);
		threads_left.clear();
	}

	thread_handles;
	int i = 0;
}
