#pragma once

#include "Camera.h"

/* 낵 ㅔㄱ임내에서 사용될 수 있는 뷰, 투영변홚 애렬을 가지고 잇는다. */
BEGIN(Engine)

class CPipeline final : public CBase
{
	DECLARE_SINGLETON(CPipeline)
public:
	enum TYPE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };
public:
	CPipeline();
	virtual ~CPipeline() = default;
public:
	_matrix Get_Transform(TYPE eType) {
		return eType == D3DTS_VIEW ? XMLoadFloat4x4(&m_ViewMatrix) : XMLoadFloat4x4(&m_ProjMatrix);
	}

	_vector Get_CamPosition() {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_ViewMatrix)).r[3];
	}

	void Set_Transform(TYPE eType, _fmatrix StateMatrix) {
		if (eType == D3DTS_VIEW)
			XMStoreFloat4x4(&m_ViewMatrix, StateMatrix);
		else
			XMStoreFloat4x4(&m_ProjMatrix, StateMatrix);
	}

private:
	_float4x4			m_ViewMatrix;
	_float4x4			m_ProjMatrix;

public:
	virtual void Free() override;
};

END