#pragma once
#include "Base.h"

BEGIN(Engine)

class CData_Manager final : public CBase
{

private:
	CData_Manager();
	virtual ~CData_Manager() = default;

public:
	HRESULT					   Initialize();
	HRESULT					   Add_PrototypeTag(const wstring& strProtoTypeTag, _bool bModelType);
	HRESULT					   Add_LayerTag(const wstring& strLayerTag);

	vector<wstring>&		   Get_LayerTags() { return m_vecLayerTags;}
	vector<wstring>&		   Get_VecTags() { return m_vecTags; }

	map<const wstring, _bool>& Get_ObjectTags() { return m_ObjectTags;}

private:
	vector<wstring>		m_vecTags;
	
	vector<wstring>		m_vecLayerTags;
	map<const wstring, _bool> m_ObjectTags;

public:
	static CData_Manager* Create();
	virtual void Free() override;

};

END

