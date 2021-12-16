#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;
public:
	HRESULT Initialize(const char* pChannelName);
	HRESULT Add_KeyFrameDesc(KEYFRAMEDESC* pKeyFrameDesc);

public:
	vector<KEYFRAMEDESC*> Get_KeyFrames() {
		return m_KeyFrames;
	}

	const char* Get_Name() const {
		return m_szChannelName;
	}

	_matrix Get_TransformationMatrix() {
		return XMLoadFloat4x4(&m_TransformationMatrix);
	}

	_uint Get_CurrentKeyFrame() const {
		return m_iCurrentKeyFrame;
	}

	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

	void Set_CurrentKeyFrame(_uint iCurrentKeyFrame) {
		m_iCurrentKeyFrame = iCurrentKeyFrame;
	}

private:
	char						m_szChannelName[MAX_PATH] = "";
	vector<KEYFRAMEDESC*>		m_KeyFrames;
	_float4x4					m_TransformationMatrix;
	_uint						m_iCurrentKeyFrame = 0;

public:
	static CChannel* Create(const char* pChannelName);
	virtual void Free() override;
};

END
