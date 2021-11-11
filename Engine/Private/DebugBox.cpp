#include "..\Public\DebugBox.h"

CDebugBox::CDebugBox(_float3 size)
	: m_Size(size)
{
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	_float3 lines[24];
	CreateVertex(lines);
	
	__super::Initialize(lines, 12);
}

CDebugBox::~CDebugBox()
{
}

void CDebugBox::Render()
{
	__super::Render();
}

void CDebugBox::CreateVertex(_float3* lines)
{
	_float3 max{ m_Size.x / 2.f, m_Size.y / 2.f , m_Size.z / 2.f };
	_float3 min{ -m_Size.x / 2.f, -m_Size.y / 2.f , -m_Size.z / 2.f };

	_float3 pos[8];

	// ¾Õ - ÁÂÇÏ ÁÂ»ó ¿ìÇÏ ¿ì»ó
	pos[0] = _float3(min.x, min.y, min.z);
	pos[1] = _float3(min.x, max.y, min.z);
	pos[2] = _float3(max.x, min.y, min.z);
	pos[3] = _float3(max.x, max.y, min.z);

	// µÚ - ÁÂÇÏ ÁÂ»ó ¿ìÇÏ ¿ì»ó
	pos[4] = _float3(min.x, min.y, max.z);
	pos[5] = _float3(min.x, max.y, max.z);
	pos[6] = _float3(max.x, min.y, max.z);
	pos[7] = _float3(max.x, max.y, max.z);

	_float3 temp[] =
	{
		// ¾Õ¸é
		pos[0],pos[1], pos[1],pos[3],
		pos[3],pos[2], pos[2],pos[0],

		// µÞ¸é
		pos[4],pos[5], pos[5],pos[7],
		pos[7],pos[6], pos[6],pos[4],

		// ¿ÞÂÊ ¿·
		pos[0],pos[4], pos[1],pos[5],
		// ¿À¸¥ÂÊ ¿·
		pos[2],pos[6], pos[3],pos[7]
	};

	for (int i = 0; i < 24; i++)
		lines[i] = temp[i];
}
