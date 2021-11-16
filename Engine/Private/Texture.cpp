#include "..\Public\Texture.h"

CTexture::CTexture()
{
	vertexCount = 4;
	indexCount = 6;
	shader = new CShader(L"../../Assets/Shader/TextureTest.hlsl");
	worldBuffer = new WorldBuffer();
	colorBuffer = new ColorBuffer;
	vertices = new VertexTexture[vertexCount];
	vertices[0].Position = _float3(0, 0, 0);
	vertices[1].Position = _float3(0, 1, 0);
	vertices[2].Position = _float3(1, 0, 0);
	vertices[3].Position = _float3(1, 1, 0);

	vertices[0].Uv = _float2(0, 1);
	vertices[1].Uv = _float2(0, 0);
	vertices[2].Uv = _float2(1, 1);
	vertices[3].Uv = _float2(1, 0);

	vertices[0].Normal = _float3(0, 0, -1);
	vertices[1].Normal = _float3(0, 0, -1);
	vertices[2].Normal = _float3(0, 0, -1);
	vertices[3].Normal = _float3(0, 0, -1);

	indices = new UINT[indexCount]{ 0, 1, 2, 2, 1, 3 };

	// CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // ��� ��������� ���� ����
		desc.ByteWidth = sizeof(VertexTexture) * vertexCount; // ���� ���ۿ� �� �������� ũ��
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // �긦 ���ؼ� ���� �� lock ���
		data.pSysMem = vertices; // �� �������� �ּ�

		HRESULT hr = CEngine::GetInstance()->GetDevice()->CreateBuffer(
			&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr)); // �����Ǹ� hr 0���� ū �� �Ѿ��
	}

	// CreateIndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // ��� ��������� ���� ����
		desc.ByteWidth = sizeof(UINT) * indexCount; // ���� ���ۿ� �� �������� ũ��
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // �긦 ���ؼ� ���� �� lock ���
		data.pSysMem = indices; // �� �������� �ּ�

		HRESULT hr = CEngine::GetInstance()->GetDevice()->CreateBuffer(
			&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr)); // �����Ǹ� hr 0���� ū �� �Ѿ��
	}

	// Load SRV
	{
		HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
			CEngine::GetInstance()->GetDevice(),
			(L"../../Assets/Texture/Isu.png")/*.c_str()*/,
			NULL, // �о�帱�� ������ NULL�̸� �⺻��
			NULL,
			&srv,
			NULL // ���⼭ hr ���� �޾Ƶ���
		);
		assert(SUCCEEDED(hr));
	}
}


CTexture::~CTexture()
{
}

void CTexture::Set()
{
	static float t = 0.0f;
	t += 0.0001f;
	XMMATRIX newMat = XMMatrixRotationX(t);

	//XMMATRIX mSpin = XMMatrixRotationX(-t);
	//XMMATRIX mTranslate = XMMatrixTranslation(5.0f,-5.0f, 0.0f);
	//XMMATRIX mScale = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	//newMat = mScale * mSpin * mTranslate;

	worldBuffer->SetMatrix(newMat);
	//colorBuffer->SetPSBuffer(0);
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	CEngine::GetInstance()->GetDeviceContext()->PSSetShaderResources(
		0, // 0��
		1, // ���� 
		&srv);
}

void CTexture::Render()
{
	UINT stride = sizeof(VertexTexture); // �׸� ũ��
	UINT offset = 0;

	// vertex buffer ������ �� �� ����
	// IA �ٴ� ���� ���߿� �������ֽǲ�
	CEngine::GetInstance()->GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	CEngine::GetInstance()->GetDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// �׸� ��� ����
	CEngine::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// ������ �׸��� ��
	CEngine::GetInstance()->GetDeviceContext()->DrawIndexed(indexCount, 0, 0);
}
