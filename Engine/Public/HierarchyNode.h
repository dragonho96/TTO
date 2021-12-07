#pragma once

#include "Base.h"

BEGIN(Engine)

class CHierarchyNode final : public CBase
{
public:
	CHierarchyNode();
	virtual ~CHierarchyNode() = default;
public:
	_uint Get_Depth() const {
		return m_iDepth;
	}

	const char* Get_Name() const {
		return m_szNodeName;
	}

public:
	HRESULT Initialize(const char* pNodeName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, _uint iDepth);

private:
	char				m_szNodeName[MAX_PATH] = "";
	_float4x4			m_TransformationMatrix;
	CHierarchyNode*		m_pParent = nullptr;
	_uint				m_iDepth = 0;

public:
	static CHierarchyNode* Create(const char* pNodeName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, _uint iDepth);
	virtual void Free() override;

};

END