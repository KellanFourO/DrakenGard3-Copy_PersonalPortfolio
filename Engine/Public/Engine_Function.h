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

}

#endif // Engine_Function_h__
