#pragma once
#include "Base.h"
#include "Model.h"

BEGIN(Engine)

class CModelManager final : public CBase
{
	DECLARE_SINGLETON(CModelManager)
private:
	CModelManager();
	virtual ~CModelManager() = default;
	virtual void Free() override;

public:
	class CComponent* CloneModel(string pMeshFilePath, string pMeshFileName, string pShaderFilePath = "../../Assets/Shader/Shader_Mesh.fx", _bool meshCollider = false, void* pArg = nullptr);
	CTexture* GetDissoveTex() { return m_pDissolveTex; }

private:
	unordered_map<string, CModel*> m_mapModel;
private:
	class CTexture* m_pDissolveTex = nullptr;
};

END