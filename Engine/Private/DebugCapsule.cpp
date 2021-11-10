#include "..\Public\DebugCapsule.h"

CDebugCapsule::CDebugCapsule(float radius, float height, AXIS axis, int stackCount, int sliceCount)
{
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	m_fRadius = radius;
	m_fHeight = height;
	m_iStackCount = stackCount;
	m_iSliceCount = sliceCount;
	m_eAxis = axis;

	m_pLines = nullptr;

	CreateVertex();

	__super::Initialize(m_pLines, m_iLineCount);
}

CDebugCapsule::~CDebugCapsule()
{
	SafeDeleteArray(m_pLines);
}

void CDebugCapsule::CreateVertexFilled()
{
	if (m_pLines != NULL)
		SafeDeleteArray(m_pLines);


	float phiStep = 3.14f / m_iStackCount;
	float thetaStep = 2.0f * 3.14f / (float)m_iSliceCount;

	// Create Vertex
	{
		int vertexCount = 2 + (m_iSliceCount + 1) * (m_iStackCount - 1);
		_float3* vertices = new _float3[vertexCount];

		UINT index = 0;
		for (UINT i = 0; i <= m_iStackCount; i++) {

			switch (m_eAxis)
			{
			case CDebugCapsule::AXIS::AXIS_X:
			{
				// 위와 밑의 꼭지점 처리
				if (i == 0 || i == m_iStackCount) {
					vertices[index] = _float3(i == 0 ?
						m_fRadius + 0.5f * m_fHeight : m_fHeight * -0.5f
						- m_fRadius, 0, 0);

					index++;
				}
				else if (i <= m_iStackCount / 2) {
					float phi = i * phiStep;
					for (UINT j = 0; j <= m_iSliceCount; j++) {
						float theta = j * thetaStep;
						vertices[index] = _float3(
							(m_fRadius * cosf(phi) + 0.5f * m_fHeight),
							(m_fRadius * sinf(phi) * cosf(theta)),
							(m_fRadius * sinf(phi) * sinf(theta)));

						index++;
					}
				}
				else {
					float phi = i * phiStep;
					for (UINT j = 0; j <= m_iSliceCount; j++) {
						float theta = j * thetaStep;
						vertices[index] = _float3(
							(m_fRadius * cosf(phi) - 0.5f * m_fHeight),
							(m_fRadius * sinf(phi) * cosf(theta)),
							(m_fRadius * sinf(phi) * sinf(theta)));

						index++;
					}
				}
			}
			break;
			case CDebugCapsule::AXIS::AXIS_Y:
			{
				// 위와 밑의 꼭지점 처리
				if (i == 0 || i == m_iStackCount) {
					vertices[index] = _float3(0, i == 0 ?
						m_fRadius + 0.5f * m_fHeight : m_fHeight * -0.5f
						- m_fRadius, 0);

					index++;
				}
				else if (i <= m_iStackCount / 2) {
					float phi = i * phiStep;
					for (UINT j = 0; j <= m_iSliceCount; j++) {
						float theta = j * thetaStep;
						vertices[index] = _float3(
							(m_fRadius * sinf(phi) * cosf(theta)),
							(m_fRadius * cosf(phi) + 0.5f * m_fHeight),
							(m_fRadius * sinf(phi) * sinf(theta)));

						index++;
					}
				}
				else {
					float phi = i * phiStep;
					for (UINT j = 0; j <= m_iSliceCount; j++) {
						float theta = j * thetaStep;
						vertices[index] = _float3(
							(m_fRadius * sinf(phi) * cosf(theta)),
							(m_fRadius * cosf(phi) - 0.5f * m_fHeight),
							(m_fRadius * sinf(phi) * sinf(theta)));

						index++;
					}
				}
			}
			break;
			case CDebugCapsule::AXIS::AXIS_Z:
			{
				// 위와 밑의 꼭지점 처리
				if (i == 0 || i == m_iStackCount) {
					vertices[index] = _float3(0, 0, i == 0 ?
						m_fRadius + 0.5f * m_fHeight : m_fHeight * -0.5f
						- m_fRadius);

					index++;
				}
				else if (i <= m_iStackCount / 2) {
					float phi = i * phiStep;
					for (UINT j = 0; j <= m_iSliceCount; j++) {
						float theta = j * thetaStep;
						vertices[index] = _float3(
							(m_fRadius * sinf(phi) * cosf(theta)),
							(m_fRadius * sinf(phi) * sinf(theta)),
							(m_fRadius * cosf(phi) + 0.5f * m_fHeight));

						index++;
					}
				}
				else {
					float phi = i * phiStep;
					for (UINT j = 0; j <= m_iSliceCount; j++) {
						float theta = j * thetaStep;
						vertices[index] = _float3(
							(m_fRadius * sinf(phi) * cosf(theta)),
							(m_fRadius * sinf(phi) * sinf(theta)),
							(m_fRadius * cosf(phi) - 0.5f * m_fHeight));

						index++;
					}
				}
			}
			break;
			}


		}

		m_iLineCount = (2 + (m_iStackCount - 2) + (m_iStackCount - 1)) * m_iSliceCount;
		m_pLines = new _float3[m_iLineCount * 2];

		index = 0;
		// 윗면
		for (UINT i = 1; i <= m_iSliceCount; i++) {
			m_pLines[index++] = vertices[0];
			m_pLines[index++] = vertices[i];
		}

		// 옆면
		UINT baseIndex = 1; // 위 꼭지점 빼려고
		UINT ringVertexCount = m_iSliceCount + 1;
		// m_iStackCount - 2 인 이유 정점 4개 써서 위 꼭지 아래꼭지 빼고 -1 해줘야함
		for (UINT i = 0; i < m_iStackCount - 1; i++) {

			for (UINT j = 0; j < m_iSliceCount; j++) {
				// 세로줄
				if (i < m_iStackCount - 2)
				{
					m_pLines[index++] = vertices[baseIndex + i * ringVertexCount + j];
					m_pLines[index++] = vertices[baseIndex + (i + 1) * ringVertexCount + j];
				}

				// 가로줄
				m_pLines[index++] = vertices[baseIndex + i * ringVertexCount + j];
				m_pLines[index++] = vertices[baseIndex + i * ringVertexCount + (j + 1)];
			}
		}

		// 아랫면
		UINT southPoleIndex = vertexCount - 1;
		baseIndex = southPoleIndex - ringVertexCount;
		for (UINT i = 0; i < m_iSliceCount; i++) {
			m_pLines[index++] = vertices[southPoleIndex];
			m_pLines[index++] = vertices[baseIndex + i];
		}

		SafeDeleteArray(vertices);
	}
}

void CDebugCapsule::CreateVertex()
{
	if (m_pLines != nullptr)
		SafeDeleteArray(m_pLines);

	int sliceCount = 36;

	float phiStep = 2.0f * 3.14f / (float)sliceCount;

	int vertexCount = sliceCount * 4 + 4;
	_float3* vertices = new _float3[vertexCount];

	UINT index = 0;
	float phi = 0.f;
	// x = 0
	for (UINT i = 0; i < sliceCount / 2; i++) {
		phi = i * phiStep;
		vertices[index] = _float3(0, (m_fRadius * cosf(phi)) + m_fHeight / 2.f, (m_fRadius * sinf(phi)));
		index++;
	}
	vertices[index] = _float3(0, (m_fRadius * cosf(phi)) - m_fHeight / 2.f, (m_fRadius * sinf(phi)));
	index++;
	// x = 0
	phi = 0.f;
	for (UINT i = sliceCount / 2; i < sliceCount; i++) {
		phi = i * phiStep;
		vertices[index] = _float3(0, (m_fRadius * cosf(phi)) - m_fHeight / 2.f, (m_fRadius * sinf(phi)));
		index++;
	}
	vertices[index] = _float3(0, (m_fRadius * cosf(phi)) + m_fHeight / 2.f, (m_fRadius * sinf(phi)));
	index++;


	// y = 0
	phi = 0.f;
	for (UINT i = 0; i < sliceCount; i++) {
		phi = i * phiStep;
		vertices[index] = _float3((m_fRadius * cosf(phi)), m_fHeight / 2.f, (m_fRadius * sinf(phi)));
		index++;
	}
	// y = 0
	phi = 0.f;
	for (UINT i = 0; i < sliceCount; i++) {
		phi = i * phiStep;
		vertices[index] = _float3((m_fRadius * cosf(phi)), -m_fHeight / 2.f, (m_fRadius * sinf(phi)));
		index++;
	}

	// z = 0
	 phi = 0.f;
	for (UINT i = 0; i < sliceCount / 2; i++) {
		phi = i * phiStep;
		vertices[index] = _float3((m_fRadius * cosf(phi)), (m_fRadius * sinf(phi)) + m_fHeight / 2.f, 0);
		index++;
	}
	vertices[index] = _float3((m_fRadius * cosf(phi)), (m_fRadius * sinf(phi)) - m_fHeight / 2.f, 0);
	index++;
	// z = 0
	 phi = 0.f;
	for (UINT i = sliceCount / 2; i < sliceCount; i++) {
		phi = i * phiStep;
		vertices[index] = _float3((m_fRadius * cosf(phi)), (m_fRadius * sinf(phi)) - m_fHeight / 2.f, 0);
		index++;
	}
	vertices[index] = _float3((m_fRadius * cosf(phi)), (m_fRadius * sinf(phi)) + m_fHeight / 2.f, 0);
	index++;

	m_iLineCount = vertexCount;
	m_pLines = new _float3[vertexCount * 2];
	index = 0;
	for (UINT i = 0; i < sliceCount + 2; i++) {
		m_pLines[index++] = vertices[i];
		m_pLines[index++] = i == (sliceCount + 2) - 1 ? vertices[0] : vertices[i + 1];
	}

	for (UINT i = sliceCount + 2; i < sliceCount * 2 + 2; i++) {
		m_pLines[index++] = vertices[i];
		m_pLines[index++] = i == (sliceCount * 2 + 2) - 1 ? vertices[sliceCount + 2] : vertices[i + 1];
	}

	for (UINT i = sliceCount * 2 + 2; i < sliceCount * 3 + 2; i++) {
		m_pLines[index++] = vertices[i];
		m_pLines[index++] = i == (sliceCount * 3 + 2) - 1 ? vertices[sliceCount * 2 + 2] : vertices[i + 1];
	}

	for (UINT i = sliceCount * 3 + 2; i < sliceCount * 4 + 4; i++) {
		m_pLines[index++] = vertices[i];
		m_pLines[index++] = i == (sliceCount * 4 + 4) - 1 ? vertices[sliceCount * 3 + 2] : vertices[i + 1];
	}

	SafeDeleteArray(vertices);
}

void CDebugCapsule::Render()
{
	__super::Render();
}
