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

HRESULT CData_Manager::Add_EffectTexutreTag(const wstring& strTextureTag)
{
	auto iter = find(m_vecEffectTextureTags.begin(), m_vecEffectTextureTags.end(), strTextureTag);

	if (iter != m_vecEffectTextureTags.end())
		return E_FAIL;

	m_vecEffectTextureTags.push_back(strTextureTag);

	return S_OK;
}

HRESULT CData_Manager::Add_EffectMeshTag(const wstring& strMeshModelTag)
{
	auto iter = find(m_vecEffectMeshTags.begin(), m_vecEffectMeshTags.end(), strMeshModelTag);

	if (iter != m_vecEffectMeshTags.end())
		return E_FAIL;

	m_vecEffectMeshTags.push_back(strMeshModelTag);

	return S_OK;
}

HRESULT CData_Manager::Add_ModelData(const wstring& strModelDataTag, MODELDATA* ModelData)
{
	auto iter = m_ModelDatas.find(strModelDataTag);

	if (iter != m_ModelDatas.end())
		return E_FAIL;

	m_ModelDatas.emplace(strModelDataTag, ModelData);

	return S_OK;
}

MODELDATA* CData_Manager::Get_ModelData_InKey(const wstring& strModelDataTag)
{
	auto iter = m_ModelDatas.find(strModelDataTag);

	return iter->second;
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

	for (auto& pModelData : m_ModelDatas)
	{
		Safe_Delete(pModelData.second);
	}

	m_ModelDatas.clear();
}
