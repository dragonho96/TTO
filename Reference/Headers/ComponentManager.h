#pragma once

#include "Renderer.h"
#include "Renderer.h"
#include "Transform.h"
#include "RectTransform.h"

#include "VIBuffer_Rect.h"
#include "VIBuffer_RectUI.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_LineBox.h"
#include "VIBuffer_LineCapsule.h"
#include "VIBuffer_LineSphere.h"

#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"

#include "Text.h"
#include "Texture.h"

#include "Model.h"

BEGIN(Engine)

class CComponentManager final : public CBase
{
	DECLARE_SINGLETON(CComponentManager)
private:
	CComponentManager();
	virtual ~CComponentManager() = default;
	virtual void Free() override;

public:
	HRESULT ReserveManager(_uint iNumScenes);
	HRESULT AddPrototype(_uint iSceneIndex, string pPrototypeTag, CComponent* pPrototype);
	CComponent* CloneComponent(_uint iSceneIndex, string pPrototypeTag, void* pArg);
	void Clear(_uint iSceneIndex);

private:
	unordered_map<string, CComponent*>*			m_pPrototypes = nullptr;
	typedef unordered_map<string, CComponent*>	PROTOTYPES;
private:
	_uint		m_iNumScenes = 0;
public:
	CComponent* FindPrototype(_uint iSceneIndex, string pPrototypeTag);
};

END