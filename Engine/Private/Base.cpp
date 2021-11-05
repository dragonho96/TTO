#include "..\public\Base.h"

USING(Engine)

CBase::CBase()
	: m_dwRefCnt(0)
{
}

_uint CBase::AddRef()
{
	return ++m_dwRefCnt;	
}

_uint CBase::Release()
{
	if (0 == m_dwRefCnt)
	{	
		Free();
		delete this;
		return 0;	
	}
	else
		return m_dwRefCnt--;
}

