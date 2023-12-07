#pragma once

#include "Engine_Defines.h"
#include "Json/Json_Utility.h"

/* 레퍼런스카운트를 관리하기위한 기능을 제공한다. */

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