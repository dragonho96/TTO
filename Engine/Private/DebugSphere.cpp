#include "..\Public\DebugSphere.h"

CDebugSphere::CDebugSphere(float radius, int sliceCount)
{
	this->sliceCount = sliceCount;
	this->range = radius;
	lines = NULL;
	CreateVertex();

	__super::Initialize(lines, lineCount);
}

CDebugSphere::~CDebugSphere()
{
	SafeDeleteArray(lines);
}

void CDebugSphere::Render()
{
	__super::Render();
}

void CDebugSphere::CreateVertex()
{
	if (lines != NULL)
		SafeDeleteArray(lines);

	float phiStep = 2.0f * 3.14f / (float)sliceCount;

	// Create Vertex
	{
		int vertexCount = sliceCount * 3;
		_float3* vertices = new _float3[vertexCount];

		UINT index = 0;
		// x = 0
		for (UINT i = 0; i < sliceCount; i++) {
			float phi = i * phiStep;
			vertices[index] = _float3(0, (range * cosf(phi)), (range * sinf(phi)));
			index++;
		}
		// y = 0
		for (UINT i = 0; i < sliceCount; i++) {
			float phi = i * phiStep;
			vertices[index] = _float3((range * cosf(phi)), 0, (range * sinf(phi)));
			index++;
		}
		// z = 0
		for (UINT i = 0; i < sliceCount; i++) {
			float phi = i * phiStep;
			vertices[index] = _float3((range * cosf(phi)), (range * sinf(phi)), 0);
			index++;
		}

		lineCount = sliceCount * 3;
		lines = new _float3[lineCount * 2];

		index = 0;
		for (UINT i = 0; i < sliceCount; i++) {
			lines[index++] = vertices[i];
			lines[index++] = i == sliceCount - 1 ? vertices[0] : vertices[i + 1];
		}
		for (UINT i = sliceCount; i < sliceCount * 2; i++) {
			lines[index++] = vertices[i];
			lines[index++] = i == sliceCount * 2 - 1 ? vertices[sliceCount] : vertices[i + 1];
		}
		for (UINT i = sliceCount * 2; i < sliceCount * 3; i++) {
			lines[index++] = vertices[i];
			lines[index++] = i == sliceCount * 3 - 1 ? vertices[sliceCount * 2] : vertices[i + 1];
		}

		SafeDeleteArray(vertices);
	}

	this->vertexCount = lineCount * 2;
}
