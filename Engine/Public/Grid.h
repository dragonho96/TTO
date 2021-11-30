#pragma once

#include "GameObject.h"
#include "Node.h"

BEGIN(Engine)
class ENGINE_DLL CGrid : public CGameObject
{
public:
	typedef struct tagGridDesc
	{
		_uint iSizeX;
		_uint iSizeZ;
		_float iSizeInterval;
	}GRIDDESC;

protected:
	explicit CGrid(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGrid(const CGrid& rhs);
	virtual ~CGrid() = default;
public:
	static CGrid* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	void SetUpGrid();

private:
	GRIDDESC		m_Desc;
	vector<CNode*>	m_Nodes;

};

END