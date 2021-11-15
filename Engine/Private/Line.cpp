#include "..\Public\Line.h"
#include "Shader.h"
#include "Transform.h"

CLine::CLine()
	: m_pVertices(nullptr)
	, position(0.f, 0.f, 0.f), rotation(0.f, 0.f, 0.f)
	, scale(1.f, 1.f, 1.f)
{
	world = XMMatrixIdentity();
}


CLine::~CLine()
{
	SafeRelease(m_pTransform);
	SafeDeleteArray(m_pVertices);
}

void CLine::Initialize(_float3* lines, UINT lineCount)
{
	// Create Shader
	m_pShader = make_unique<CShader>(L"../../Assets/Shader/Tutorial05.fx");
	m_pTransform = CTransform::Create(CEngine::GetInstance()->GetDevice(), CEngine::GetInstance()->GetDeviceContext());

	if (m_eTopology == D3D11_PRIMITIVE_TOPOLOGY_LINELIST)
		vertexCount = lineCount * 2;
	else if (m_eTopology == D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP)
		vertexCount = lineCount + 1;


	// Create Vertex Data
	
	// Set vertextCount in child
	// vertexCount = lineCount;

	m_pVertices = new SimpleVertex[vertexCount];

	for (UINT i = 0; i < vertexCount; i++) {
		m_pVertices[i].Pos = lines[i];
		m_pVertices[i].Color = { 0.f, 1.f, 0.f, 1.f };
	}

	// Create Vertex Buffer

	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_DEFAULT; // ��� ��������� ���� ����
	desc.ByteWidth = sizeof(SimpleVertex) * vertexCount; // ���� ���ۿ� �� �������� ũ��
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA  data = { 0 }; // �긦 ���ؼ� ���� �� lock ���
	data.pSysMem = m_pVertices; // �� �������� �ּ�

	HRESULT hr = CEngine::GetInstance()->GetDevice()->CreateBuffer(
		&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr)); // �����Ǹ� hr 0���� ū �� �Ѿ��
}

void CLine::Render()
{
	UINT stride = sizeof(SimpleVertex); // �׸� ũ��
	UINT offset = 0;

	ConstantBuffer cb1;
	_float4x4 matrix = m_pTransform->GetMatrix();
	cb1.mWorld = XMMatrixTranspose(XMLoadFloat4x4(&matrix));
	cb1.mView = XMMatrixTranspose(CEngine::GetInstance()->GetViewMatrix());
	cb1.mProjection = XMMatrixTranspose(CEngine::GetInstance()->GetProjectionMatrix());

	CEngine::GetInstance()->GetDeviceContext()->UpdateSubresource(
		CEngine::GetInstance()->GetConstantBuffer(), 0, NULL, &cb1, 0, 0);

	CEngine::GetInstance()->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	CEngine::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(m_eTopology);
	m_pShader->Render();
	// ������ �׸��� ��
	CEngine::GetInstance()->GetDeviceContext()->Draw(vertexCount, 0);
}
