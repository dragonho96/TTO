#pragma once


namespace Engine
{
	template<typename T>
	using ComRef = Microsoft::WRL::ComPtr<T>;

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T>
	void SafeDelete(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	void SafeDeleteArray(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete[] Pointer;
			Pointer = nullptr;
		}
	}

	/* 레퍼런스 카운트를 감소시킨다. or 삭제한다. */
	template<typename T>
	unsigned long SafeRelease(T& pInstance)
	{
		unsigned long dwRefCnt = 0;

		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release();

			if (0 == dwRefCnt)
				pInstance = nullptr;
		}
		return dwRefCnt;
	}

	template<typename T>
	unsigned long SafeAddRef(T& pInstance)
	{
		if (nullptr == pInstance)
			return 0;

		unsigned long dwRefCnt = 0;
		
		dwRefCnt = pInstance->AddRef();
	
		return dwRefCnt;
	}	

	class CTagFinder
	{
	public:
		explicit CTagFinder(const wchar_t* pTag)
			: m_pTargetTag(pTag) {		}
		~CTagFinder() {}

	public:
		template<typename T>
		_bool		operator()(const T& Pair)
		{
			if (0 == lstrcmp(m_pTargetTag, Pair.first))
				return true;

			return false;
		}

	private:
		const wchar_t*	m_pTargetTag;

	};
}
