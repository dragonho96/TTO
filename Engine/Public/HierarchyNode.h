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

	const _float4x4 Get_TransformationMatrix() const {
		return m_TransformationMatrix;
	}

	const _float4x4 Get_CombinedTransformationMatrix() const {
		return m_CombinedTransformationMatrix;
	}

public:
	HRESULT Initialize(const char* pNodeName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, _uint iDepth);
	HRESULT Add_Channel(_uint iAnimationIndex, class CChannel* pChannel);
	void Update_CombinedTransformationMatrix(_uint iAnimationIndex);
	void Reserve_Channels(_uint iNumAnimation);

private:
	char				m_szNodeName[MAX_PATH] = "";
	_float4x4			m_TransformationMatrix;
	_float4x4			m_CombinedTransformationMatrix;
	CHierarchyNode*		m_pParent = nullptr;
	_uint				m_iDepth = 0;

private:
	vector<class CChannel*>			m_Channels;

public:
	static CHierarchyNode* Create(const char* pNodeName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, _uint iDepth);
	virtual void Free() override;

};

END