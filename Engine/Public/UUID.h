#pragma once
#include "Base.h"
#include <random>
#include <xhash>

BEGIN(Engine)

class ENGINE_DLL CUUID : public CBase
{
public:
	CUUID();
	CUUID(uint64_t uuid);
	CUUID(const CUUID&) = default;

	uint64_t operator ()() { return m_UUID; }

private:
	uint64_t	 m_UUID;

	// Inherited via CBase
	virtual void Free() override;
};



END
