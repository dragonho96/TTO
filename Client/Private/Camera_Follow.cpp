#include "stdafx.h"
#include "..\Public\Camera_Follow.h"
#include "Engine.h"

unsigned int APIENTRY GrenadeShake(void* pArg)
{
	CCamera_Follow*		pCamera = (CCamera_Follow*)pArg;

	EnterCriticalSection(&pCamera->Get_CS());

	pCamera->Shake_Grenade();

	LeaveCriticalSection(&pCamera->Get_CS());

	return 0;
}

USING(Client)
CCamera_Follow::CCamera_Follow(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CCamera_Follow::CCamera_Follow(const CCamera_Follow & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Follow::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Follow::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	InitializeCriticalSection(&m_CS);

	list<class CGameObject*> list = CEngine::GetInstance()->GetGameObjectInLayer(0, "Player");
	if (list.size() <= 0)
		return E_FAIL;

	CGameObject* m_pGameObject = list.front();
	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pGameObject->GetComponent("Com_Transform"));

	SetUpComponents(0, "Prototype_Transform", "Com_Transform_Target", (CComponent**)&m_pTargetTransform);
	return S_OK;
}

_uint CCamera_Follow::Update(_double TimeDelta)
{
	if (nullptr == m_pTransformCom)
		return -1;

	_vector targetPos = m_pPlayerTransform->GetState(CTransform::STATE_POSITION) + m_CurShakeOffset + m_CurGrenadeShakeOffset;
	m_pTargetTransform->SetState(CTransform::STATE_POSITION, targetPos);


	// Q || E 누르면 m_pTargetLook을 회전시긴다
	if (GetActiveWindow() == g_hWnd && m_bRolling)
	{
		if (m_pEngine->GetMouseMoveValue().z > 0)
			m_fFollowLength -= (TimeDelta * 3.f);
		if (m_pEngine->GetMouseMoveValue().z < 0)
			m_fFollowLength += (TimeDelta * 3.f);

		if (m_fFollowLength < 3.f)
			m_fFollowLength = 3.f;
		else if (m_fFollowLength > 15.f)
			m_fFollowLength = 15.f;


		float speed = 8.f;
		if (m_pEngine->IsKeyPressed('Q'))
			targetAngle -= XMConvertToRadians(45.f) * TimeDelta * speed;
		else if (m_pEngine->IsKeyPressed('E'))
			targetAngle += XMConvertToRadians(45.f) * TimeDelta * speed;

		_float	fFollowSpeed = TimeDelta * 6.f;

		angle = Lerp(angle, targetAngle, fFollowSpeed);

		_vector vTargetPos = m_pTargetTransform->GetState(CTransform::STATE_POSITION);

		_vector vInvTargetLook = _vector{ 0, 0, 1 } *-m_fFollowLength;

		// Right-Axis Rotation
		XMMATRIX matRot;
		matRot = XMMatrixRotationAxis(m_pTargetTransform->GetState(CTransform::STATE_RIGHT), XMConvertToRadians(60.f));
		vInvTargetLook = XMVector3TransformNormal(ToVec3(vInvTargetLook), matRot);

		// Up-Axis Rotation
		matRot = XMMatrixRotationAxis(m_pTargetTransform->GetState(CTransform::STATE_UP), angle);
		vInvTargetLook = XMVector3TransformNormal(ToVec3(vInvTargetLook), matRot);

		XMVECTOR vector = vTargetPos + vInvTargetLook;
		memcpy(&m_CameraDesc.vEye, &vector, sizeof(_float3));
		memcpy(&m_CameraDesc.vAt, &vTargetPos, sizeof(_float3));

		_vector	vLook = XMLoadFloat3(&m_CameraDesc.vAt) - XMLoadFloat3(&m_CameraDesc.vEye);
		m_pTransformCom->SetState(CTransform::STATE_LOOK, XMVector3Normalize(vLook));

		_vector	vRight = XMVector3Cross(XMLoadFloat3(&m_CameraDesc.vAxisY), vLook);
		m_pTransformCom->SetState(CTransform::STATE_RIGHT, XMVector3Normalize(vRight));

		_vector	vUp = XMVector3Cross(vLook, vRight);
		m_pTransformCom->SetState(CTransform::STATE_UP, XMVector3Normalize(vUp));

		m_pTransformCom->SetState(CTransform::STATE_POSITION, XMLoadFloat3(&m_CameraDesc.vEye));
	}

	if (CEngine::GetInstance()->IsKeyDown('G'))
		Shake_Grenade_BeginThread();

	if (!CEngine::GetInstance()->IsMousePressed(0))
		m_ShakeOffset = { 0.f, 0.f, 0.f };

	static _float fLerpSpeed = TimeDelta * 50.f;
	m_CurShakeOffset = XMVectorSetX(m_CurShakeOffset, Lerp(XMVectorGetX(m_CurShakeOffset), XMVectorGetX(m_ShakeOffset), fLerpSpeed));
	m_CurShakeOffset = XMVectorSetY(m_CurShakeOffset, Lerp(XMVectorGetY(m_CurShakeOffset), XMVectorGetY(m_ShakeOffset), fLerpSpeed));
	m_CurShakeOffset = XMVectorSetZ(m_CurShakeOffset, Lerp(XMVectorGetZ(m_CurShakeOffset), XMVectorGetZ(m_ShakeOffset), fLerpSpeed));

	static _float fGrenadeLerpSpeed = TimeDelta/* * 2.f*/;
	m_CurGrenadeShakeOffset = XMVectorSetX(m_CurGrenadeShakeOffset, Lerp(XMVectorGetX(m_CurGrenadeShakeOffset), XMVectorGetX(m_GrenadeShakeOffset), fGrenadeLerpSpeed));
	m_CurGrenadeShakeOffset = XMVectorSetY(m_CurGrenadeShakeOffset, Lerp(XMVectorGetY(m_CurGrenadeShakeOffset), XMVectorGetY(m_GrenadeShakeOffset), fGrenadeLerpSpeed));
	m_CurGrenadeShakeOffset = XMVectorSetZ(m_CurGrenadeShakeOffset, Lerp(XMVectorGetZ(m_CurGrenadeShakeOffset), XMVectorGetZ(m_GrenadeShakeOffset), fGrenadeLerpSpeed));


	return __super::Update(TimeDelta);
}

_uint CCamera_Follow::LateUpdate(_double TimeDelta)
{
	if (0 > __super::LateUpdate(TimeDelta))
		return -1;

	return _uint();
}

HRESULT CCamera_Follow::SetUp_Components()
{

	return S_OK;
}

void CCamera_Follow::Shake_Rifle()
{
	mt19937 engine(rand());
	uniform_real_distribution<_float> distribution(-0.1f, 0.1f);//
	auto generator = bind(distribution, engine);
	_float x = generator();
	_float y = generator();
	_float z = generator();

	// ADDLOG((to_string(x) + ", " + to_string(y) + ", " + to_string(z)).c_str());

	m_ShakeOffset = XMVectorSetX(m_ShakeOffset, x);
	m_ShakeOffset = XMVectorSetY(m_ShakeOffset, y);
	m_ShakeOffset = XMVectorSetZ(m_ShakeOffset, z);
}

void CCamera_Follow::Shake_Grenade()
{
	CEngine::GetInstance()->AddTimers("Timer_GrenadeShake");
	CEngine::GetInstance()->ComputeDeltaTime("Timer_GrenadeShake");

	_float fIntensity = 3.f;
	_float fInterval = 0.f;
	_double timeElapsed = 0.0;

	while (timeElapsed < 2.f)
	{

		if (fInterval > 0.05f)
		{
			mt19937 engine(rand());
			uniform_real_distribution<_float> distribution(-fIntensity, fIntensity);//
			auto generator = bind(distribution, engine);

			m_GrenadeShakeOffset = XMVectorSetX(m_GrenadeShakeOffset, generator());
			m_GrenadeShakeOffset = XMVectorSetY(m_GrenadeShakeOffset, generator());
			m_GrenadeShakeOffset = XMVectorSetZ(m_GrenadeShakeOffset, generator());
			fInterval = 0.f;
		}

		_double deltaTime = CEngine::GetInstance()->ComputeDeltaTime("Timer_GrenadeShake");
		timeElapsed += deltaTime;
		fInterval += deltaTime;
		fIntensity -= deltaTime;
	}

	m_GrenadeShakeOffset = { 0.f, 0.f, 0.f };
}

HRESULT CCamera_Follow::Shake_Grenade_BeginThread()
{
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, GrenadeShake, this, 0, nullptr);

	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

CCamera_Follow * CCamera_Follow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCamera_Follow*		pInstance = new CCamera_Follow(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Creating CCamera_Follow");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Follow::Clone(void * pArg)
{
	CCamera_Follow*		pInstance = new CCamera_Follow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Creating CCamera_Follow");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CCamera_Follow::Free()
{
	if (m_hThread)
	{
		WaitForSingleObject(m_hThread, INFINITE);

		DeleteCriticalSection(&m_CS);

		DeleteObject(m_hThread);
	}


	__super::Free();

}
