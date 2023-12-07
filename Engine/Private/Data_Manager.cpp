#include "Data_Manager.h"

CData_Manager::CData_Manager()
{
}

HRESULT CData_Manager::Initialize()
{
	//m_vecTags = new vector<wstring>;
	return S_OK;
}

HRESULT CData_Manager::Add_PrototypeTag(const wstring& strProtoTypeTag)
{
	auto iter = find(m_vecTags.begin(), m_vecTags.end(), strProtoTypeTag);

	if (iter != m_vecTags.end())
		return E_FAIL;


	m_vecTags.push_back(strProtoTypeTag);

	return S_OK;
}

CData_Manager* CData_Manager::Create()
{
	CData_Manager* pInstance = new CData_Manager;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CData_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CData_Manager::Free()
{
	//Safe_Delete(m_vecTags);
}
