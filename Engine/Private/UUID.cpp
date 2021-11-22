#include "..\Public\UUID.h"

static std::random_device s_RandomDevice;
static std::mt19937_64 s_Engine(s_RandomDevice());
static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

//static std::unordered_map<uint64_t, std::string> m_Map;
//
//static void AddToMap()
//{
//	m_Map[(uint64_t)CUUID()] = "hello";
//}

CUUID::CUUID()
	: m_UUID(s_UniformDistribution(s_Engine))
{
}

CUUID::CUUID(uint64_t uuid)
	: m_UUID(uuid)
{
}

void CUUID::Free()
{
}
