#pragma once

#include "VIBuffer.h"
#include "Texture.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	explicit CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;
public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render() override;

public:
	HRESULT CreateBuffer(void** pVertices);
	void SetHeightMapPath(string path);
	void SetTexturePath(string path);
	string GetHeightMapPath();
	string GetTexturePath();

public:
	void CreateHeightField(void ** pVertices);

private:
	_uint			m_iNumVerticesX = 0;
	_uint			m_iNumVerticesZ = 0;
	wstring			m_HeightMapPath = TEXT("../../Assets/Texture/Height.bmp");
	wstring			m_TexturePath = TEXT("../../Assets/Texture/Height.bmp");
	void*			m_pCloneVertices = nullptr;
	PxRigidStatic*	m_pTerrainActor = nullptr;
	CTexture*		m_pTexture = nullptr;
};

END