#pragma once
#include "Base.h"

BEGIN(Engine)

class CData_Manager final : public CBase
{

private:
	CData_Manager();
	virtual ~CData_Manager() = default;

public:
	HRESULT				Initialize();
	HRESULT				Add_PrototypeTag(const wstring& strProtoTypeTag);
	vector<wstring>&	Get_VecTags() { return m_vecTags; }

private:
	vector<wstring>		m_vecTags;

public:
	static CData_Manager* Create();
	virtual void Free() override;

};

END

