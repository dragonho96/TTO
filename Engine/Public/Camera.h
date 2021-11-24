#pragma once

#include "GameObject.h"

BEGIN(Engine)
class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc
	{
		/* ī�޶� ���������� �ʿ��� ������. */
		_float3		vEye;
		_float3		vAt;
		_float3		vAxisY;

		///* �������� ����. */
		//_float		fAspect;
		//_float		fFovy;
		//_float		fNear;
		//_float		fFar;

		///* ī�޶��� Ʈ���������� �ʿ��� ����. */
		//CTransform::STATEDESC			StateDesc;

	}CAMERADESC;


protected:
	explicit CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	/* ������ü�� �ʱ�ȭ�ϳ�. */
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Update(_double TimeDelta);
	virtual _uint LateUpdate(_double TimeDelta);

protected:
	class CTransform*			m_pTransformCom = nullptr;
	class CPipeline*			m_pPipeline = nullptr;

	CAMERADESC					m_CameraDesc;

public:
	//virtual CGameObject* Clone_GameObject(void* pArg = nullptr) = 0;
	virtual void Free();
};

END
