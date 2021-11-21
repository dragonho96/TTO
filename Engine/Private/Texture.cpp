#include "..\public\Texture.h"

USING(Engine)
CTexture::CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	: CComponent(pDevice, pDevice_Context)
{

}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_Textures(rhs.m_Textures)
	, m_iNumTextures(rhs.m_iNumTextures)
{
	//for (auto& pShaderResourceView : m_Textures)
	//	SafeAddRef(pShaderResourceView);

}

HRESULT CTexture::InitializePrototype(TEXTURETYPE eType, const _tchar * pTextureFilePath, _int iNumTextures)
{
	CComponent::InitializePrototype();

	if (nullptr == m_pDevice)
		return E_FAIL;

	//if (FAILED(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED)))
	//	return E_FAIL;

	m_iNumTextures = iNumTextures;

	_tchar			szTextureFileName[MAX_PATH] = L"";

	for (_int i = 0; i < iNumTextures; ++i)
	{
		ComRef<ID3D11ShaderResourceView>			pShaderResourceView = nullptr;

		wsprintf(szTextureFileName, pTextureFilePath, i);
		m_FilePath = WCHARToString(szTextureFileName);

		DirectX::ScratchImage				Image;

		HRESULT hr = 0;

		switch (eType)
		{
		case TYPE_DDS:
			hr = DirectX::LoadFromDDSFile(szTextureFileName, DirectX::DDS_FLAGS_NONE, nullptr, Image);
			break;
		case TYPE_TGA:
			hr = DirectX::LoadFromTGAFile(szTextureFileName, nullptr, Image);
			break;
		case TYPE_WIC:
			hr = DirectX::LoadFromWICFile(szTextureFileName, DirectX::WIC_FLAGS_NONE, nullptr, Image);
			break;
		}

		if (FAILED(hr))
			return E_FAIL;

		ID3D11Resource*			pResource = nullptr;

		if (FAILED(DirectX::CreateTexture(m_pDevice, Image.GetImages(), Image.GetImageCount(), Image.GetMetadata(), &pResource)))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateShaderResourceView(pResource, nullptr, &pShaderResourceView)))
			return E_FAIL;

		SafeRelease(pResource);

		m_Textures.push_back(pShaderResourceView);
	}

	return S_OK;
}

HRESULT CTexture::Initialize(void * pArg)
{
	CComponent::Initialize(pArg);

	return S_OK;
}

CTexture * CTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context, TEXTURETYPE eType, const _tchar * pTextureFilePath, _int iNumTextures)
{
	CTexture*		pInstance = new CTexture(pDevice, pDevice_Context);

	if (FAILED(pInstance->InitializePrototype(eType, pTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Creating Instance (CTextures) ");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CComponent * CTexture::Clone(void * pArg)
{
	CTexture*		pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned Instance (CTextures) ");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	CComponent::Free();

	//for (auto& pShaderResourceView : m_Textures)
	//	SafeRelease(pShaderResourceView);

	m_Textures.clear();
}
