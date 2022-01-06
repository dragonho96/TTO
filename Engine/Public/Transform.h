#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };
public:
	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec = 5.f;
		_float		fRotatePerSec;
	} TRANSFORMDESC;
private:
	explicit CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;
public:
	_vector GetState(STATE eState) {
		//return *(_vector*)&m_WorldMatrix.m[eState][0];
		_float4	output;
		switch (eState)
		{
		case Engine::CTransform::STATE_RIGHT:
			output = { m_WorldMatrix._11, m_WorldMatrix._12, m_WorldMatrix._13, m_WorldMatrix._14 };
			break;
		case Engine::CTransform::STATE_UP:
			output = { m_WorldMatrix._21, m_WorldMatrix._22, m_WorldMatrix._23, m_WorldMatrix._24 };
			break;
		case Engine::CTransform::STATE_LOOK:
			output = { m_WorldMatrix._31, m_WorldMatrix._32, m_WorldMatrix._33, m_WorldMatrix._34 };
			break;
		case Engine::CTransform::STATE_POSITION:
			output = { m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43, m_WorldMatrix._44 };
			break;
		}
		return XMLoadFloat4(&output);
	}

	_matrix GetWorldMatrixInverse() const {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}
	_matrix GetWorldMatrix() const {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}
	
	_float GetScale(STATE eState);

	void SetState(STATE eState, _fvector vData){ 
		memcpy(&m_WorldMatrix.m[eState][0], &vData, sizeof(_float3));
	}

	void SetLook(_fvector vLook);

	const _float4x4 GetMatrix() { return m_WorldMatrix; }
	void SetMatrix(const _float4x4 _matrix) { m_WorldMatrix = _matrix; }

	void SetPxMatrix(PxMat44 mat);
	void SetPxPosition(PxExtendedVec3 vec3);

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
public:
	void GoStraight(_double TimeDelta);
	void GoLeft(_double TimeDelta);
	void GoRight(_double TimeDelta);
	void GoBackWard(_double TimeDelta);
	void ChaseTarget(CTransform* pTarget);
	void ChaseTarget(_fvector vTargetPos, _double TimeDelta);
	void LookAtForLandObject(_fvector vTargetPos);
	void LookAt(_fvector vTargetPos);
	void SetUpRotation(_fvector vAxis, _float fDegree);
	void RotateAxis(_fvector vAxis, _double TimeDelta);
private:
	_float4x4				m_WorldMatrix;
	TRANSFORMDESC			m_TransformDesc; 
public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END