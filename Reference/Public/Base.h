#pragma once

#include "Engine_Defines.h"
#include "Json/Json_Utility.h"

/* ���۷���ī��Ʈ�� �����ϱ����� ����� �����Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	_ulong AddRef();
	_ulong Release();

	virtual void Write_Json(json& Out_Json) {};
	virtual void Load_FromJson(const json& In_Json) {};

private:
	_ulong			m_dwRefCnt = { 0 };

public:
	virtual void Free() {}
};

END