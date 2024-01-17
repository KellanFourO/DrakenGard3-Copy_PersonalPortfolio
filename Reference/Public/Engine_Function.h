#ifndef Engine_Function_h__
#define Engine_Function_h__

#include "Engine_Typedef.h"
#include <codecvt>
namespace Engine
{
	// ���ø��� ����� ������������ �ڷ����� ���������� ���� ��
	// ����� �ν��Ͻ�ȭ �ϱ� ���Ͽ� �����δ� Ʋ

	template<typename T>
	void	Safe_Delete(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	void	Safe_Delete_Array(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete [] Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	unsigned long Safe_Release(T& pInstance)
	{
		unsigned long		dwRefCnt = 0;

		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release();

			if (0 == dwRefCnt)
				pInstance = nullptr;
		}

		return dwRefCnt;
	}

	template<typename T>
	unsigned long Safe_AddRef(T& pInstance)
	{
		unsigned long		dwRefCnt = 0;

		if (nullptr != pInstance)		
			dwRefCnt = pInstance->AddRef();	

		return dwRefCnt;
	}

	template <typename T>
	HRESULT AddRefIfNotNull(T*& Pointer)
	{
		if (nullptr == Pointer)
			return E_FAIL;

		Safe_AddRef(Pointer);

		return S_OK;
	}

	template<typename T>
	T Random(initializer_list<T> _il)
	{
		assert(0 < _il.size());

		auto it = _il.begin();
		std::advance(it, rand() % _il.size());

		return *it;
	}

	template<typename T>
	typename std::enable_if<std::is_arithmetic<T>::value || std::is_enum<T>::value, _bool>::type
		InRange(T value, T low, T high, const string& range = "[)")
	{
		if (range == "()") {
			return value > low && value < high;
		}
		else if (range == "(]") {
			return value > low && value <= high;
		}
		else if (range == "[)") {
			return value >= low && value < high;
		}
		else if (range == "[]") {
			return value >= low && value <= high;
		}
		else
		{
			throw std::invalid_argument("Function::InRange: Invalid Range Option");
		}
	}

	template<typename T>
	typename std::enable_if<std::is_arithmetic<T>::value, T>::type
		Clamp(T low, T high, T value)
	{
		return min(max(value, low), high);
	}

#pragma region �����Ϸ��� ĳ���� �Լ�
	// static_cast
	template<typename Return, typename T>
	constexpr Return Cast(T value)
	{
		return static_cast<Return>(value);
	}

	// ���̳��� ĳ��Ʈ
	template<typename Return, typename T>
	constexpr Return DynCast(T value)
	{
		return dynamic_cast<Return>(value);
	}

	// reinterpret_cast
	template<typename Return, typename T>
	constexpr Return ReCast(T value)
	{
		return reinterpret_cast<Return>(value);
	}

	// const_cast
	template<typename Return, typename T>
	constexpr Return ConCast(T value)
	{
		return const_cast<Return>(value);
	}

	// Default Enum Ÿ�� ��ȯ��
	template<typename T, typename = enable_if_t<is_enum<T>::value>,
		typename Return = underlying_type_t<T>>
		constexpr Return ECast(T value)
	{
		return static_cast<Return>(value);
	}

	// void* ��ȯ��
	template<typename T>
	constexpr void* VPCast(T value)
	{
		return static_cast<void*>(value);
	}
#pragma endregion
}

#endif // Engine_Function_h__
