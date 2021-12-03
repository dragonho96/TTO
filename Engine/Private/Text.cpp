#include "Text.h"
#include "Engine.h"

USING(Engine)

CText::CText(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
	m_pSpriteBatch = CEngine::GetInstance()->GetSpriteBatch();
	m_pSpriteFont = CEngine::GetInstance()->GetSpriteFont();
}

CText::CText(const CText & rhs)
	: CComponent(rhs)
	, m_pSpriteBatch(rhs.m_pSpriteBatch)
	, m_pSpriteFont(rhs.m_pSpriteFont)
	, m_strText(rhs.m_strText)
	, m_vColor(rhs.m_vColor)
	, m_fLayerDepth(rhs.m_fLayerDepth)
{
}

CText * CText::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CText*	pInstance = new CText(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed To Create CText");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CText::Clone(void * pArg)
{
	CComponent*	pInstance = new CText(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone CText");
		SafeRelease(pInstance);
	}


	return pInstance;
}

void CText::Free()
{
	__super::Free();
}

HRESULT CText::InitializePrototype()
{
	return S_OK;
}

HRESULT CText::Initialize(void * pArg)
{
	if (nullptr != pArg)
		m_pTransform = (CRectTransform*)pArg;

	return S_OK;
}

HRESULT CText::Render()
{
	CRectTransform::RECTTRANSFORMDESC desc = m_pTransform->GetTransformDesc();
	_float2 vPos = { desc.posX - (desc.sizeX / 2.f), desc.posY - (desc.sizeY / 2.f) };
	CD3D11_RECT rt(desc.posX - (desc.sizeX / 2.f), desc.posY - (desc.sizeY / 2.f) , desc.posX + (desc.sizeX / 2.f),  desc.posY + (desc.sizeY / 2.f));
	
	ComRef<ID3D11RasterizerState> scissorState = nullptr;
	CD3D11_RASTERIZER_DESC rsDesc(D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE,
		0, 0.f, 0.f, TRUE, TRUE, TRUE, FALSE);
	if (FAILED(CEngine::GetInstance()->GetDevice()->CreateRasterizerState(&rsDesc, &scissorState)))
		return E_FAIL;
	m_pSpriteBatch->Begin(SpriteSortMode_Deferred,
		nullptr, nullptr, nullptr, scissorState.Get(),
		[&]()
	{
		m_pDeviceContext->RSSetScissorRects(1, &rt);
	});

	_float padding = desc.sizeX / 20.f;
	_float textWidth = desc.sizeX - padding;

	string newStr = "";
	string strCheck = "";
	_float4 size, charSize;
	XMStoreFloat4(&size, m_pSpriteFont->MeasureString(m_strText.c_str()));

	for (int i = 0; i < m_strText.length(); ++i)
	{
		string charToAdd = m_strText.substr(i, 1);
		newStr += charToAdd;
		strCheck += charToAdd;

		if (*charToAdd.begin() == '\n')
			strCheck = "";

		_float4 newStrSize;
		XMStoreFloat4(&newStrSize, m_pSpriteFont->MeasureString(strCheck.c_str()));
		if (newStrSize.x >= textWidth)
		{
			newStr += '\n';
			strCheck = "";
		}
	}

	m_pSpriteFont->DrawString(m_pSpriteBatch, newStr.c_str(), vPos, m_vColor, 0.f, XMFLOAT2(0.f, 0.f), XMFLOAT2(1.f, 1.f), DirectX::SpriteEffects_None, m_fLayerDepth);
	m_pSpriteBatch->End();

	return S_OK;
}

void CText::SetTextInfo(string text, _float layerDepth, _vector color)
{
	m_strText = text;
	m_fLayerDepth = layerDepth;
	m_vColor = color;
}
