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
		desc.Usage = D3D11_USAGE_DEFAULT; // 어떻게 저장될지에 대한 정보
		desc.ByteWidth = sizeof(VertexTexture) * vertexCount; // 정점 버퍼에 들어갈 데이터의 크기
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // 얘를 통해서 값이 들어감 lock 대신
		data.pSysMem = vertices; // 쓸 데이터의 주소

		HRESULT hr = CEngine::GetInstance()->GetDevice()->CreateBuffer(
			&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr)); // 성공되면 hr 0보다 큰 값 넘어옴
	}

	// CreateIndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // 어떻게 저장될지에 대한 정보
		desc.ByteWidth = sizeof(UINT) * indexCount; // 정점 버퍼에 들어갈 데이터의 크기
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // 얘를 통해서 값이 들어감 lock 대신
		data.pSysMem = indices; // 쓸 데이터의 주소

		HRESULT hr = CEngine::GetInstance()->GetDevice()->CreateBuffer(
			&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr)); // 성공되면 hr 0보다 큰 값 넘어옴
	}

	// Load SRV
	{
		HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
			CEngine::GetInstance()->GetDevice(),
			(L"../../Assets/Texture/Isu.png")/*.c_str()*/,
			NULL, // 읽어드릴때 설정값 NULL이면 기본값
			NULL,
			&srv,
			NULL // 여기서 hr 리턴 받아도됨
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
		0, // 0번
		1, // 갯수 
		&srv);
}

void CTexture::Render()
{
	UINT stride = sizeof(VertexTexture); // 그릴 크기
	UINT offset = 0;

	// vertex buffer 여러개 들어갈 수 있음
	// IA 붙는 이유 나중에 설명해주실꺼
	CEngine::GetInstance()->GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	CEngine::GetInstance()->GetDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// 그릴 방식 설정
	CEngine::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// 실제로 그리는 거
	CEngine::GetInstance()->GetDeviceContext()->DrawIndexed(indexCount, 0, 0);
}
