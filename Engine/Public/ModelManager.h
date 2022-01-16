#pragma once
#include "Base.h"
#include "Model.h"
#include "GameObject.h"
BEGIN(Engine)

class CModelManager final : public CBase
{
public:
	typedef struct tagModelDesc
	{
		tagModelDesc(CGameObject* _pObj, string _pMeshFilePath,
			string _pMeshFileName, string _pShaderFilePath,
			_bool _meshCollider, void* _pArg, CRITICAL_SECTION _cs)
			: pObj(_pObj), pMeshFilePath(_pMeshFilePath), pMeshFileName(_pMeshFileName),
			pShaderFilePath(_pShaderFilePath), bMeshCollider(_meshCollider), pArg(_pArg)
			,cs(_cs) {}

		CGameObject*	pObj;
		string			pMeshFilePath;
		string			pMeshFileName;
		string			pShaderFilePath;
		_bool			bMeshCollider; 
		void*			pArg;
		CRITICAL_SECTION cs;
	}MODELLOADDESC;

	DECLARE_SINGLETON(CModelManager)
private:
	CModelManager();
	virtual ~CModelManager() = default;
	virtual void Free() override;

public:
	void CloneModel(CGameObject* pObj, string pMeshFilePath, string pMeshFileName, string pShaderFilePath = "../../Assets/Shader/Shader_Mesh.fx", _bool meshCollider = false, void* pArg = nullptr);
	void CloneModelThread(CGameObject* pObj, string pMeshFilePath, string pMeshFileName, string pShaderFilePath = "../../Assets/Shader/Shader_Mesh.fx", _bool meshCollider = false, void* pArg = nullptr);
	void WaitThreads();
	
	
	CTexture* GetDissoveTex() { return m_pDissolveTex; }

private:
	unordered_map<string, CModel*> m_mapModel;
private:
	class CTexture* m_pDissolveTex = nullptr;


	set<HANDLE>				thread_handles;
	list<string>			m_CurCloningObj;
	CRITICAL_SECTION		m_CS;
	unordered_map<string, CRITICAL_SECTION> m_keys;
};

END