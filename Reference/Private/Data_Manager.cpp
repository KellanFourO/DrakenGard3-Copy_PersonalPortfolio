#include "Data_Manager.h"

CData_Manager::CData_Manager()
{
}

HRESULT CData_Manager::Initialize()
{
	//m_vecTags = new vector<wstring>;
	return S_OK;
}

HRESULT CData_Manager::Add_PrototypeTag(const wstring& strProtoTypeTag, _bool bModelType)
{
	auto iter = m_ObjectTags.find(strProtoTypeTag);

	if(iter != m_ObjectTags.end())
		return E_FAIL;

	m_ObjectTags.emplace(strProtoTypeTag, bModelType);

	return S_OK;
}

HRESULT CData_Manager::Add_LayerTag(const wstring& strLayerTag)
{
	auto iter = find(m_vecLayerTags.begin(), m_vecLayerTags.end(), strLayerTag);
		
	if(iter != m_vecLayerTags.end())
		return E_FAIL;

	m_vecLayerTags.push_back(strLayerTag);

	return S_OK;
}

HRESULT CData_Manager::Add_ModelTag(const wstring& strModelTag)
{
	auto iter = find(m_vecModelTags.begin(), m_vecModelTags.end(), strModelTag);

	if (iter != m_vecModelTags.end())
		return E_FAIL;

	m_vecModelTags.push_back(strModelTag);

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
