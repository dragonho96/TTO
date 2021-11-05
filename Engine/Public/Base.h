#pragma once

#include "Engine_Defines.h"

/* ��� Ŭ�������� �θ� �ȴ�. */
/* ���۷��� ī��Ʈ�� �����Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;
public:
	_uint AddRef();
	_uint Release();
private:
	_uint			m_dwRefCnt = 0;
public:
	/* �ش� Ŭ������ ����� �����Ѵ�. */
	virtual void Free() = 0;
};

END