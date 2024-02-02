#include "..\Public\UI_Manager.h"
#include "GameInstance.h"
#include "MyUI.h"

CUI_Manager::CUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CUI_Manager::Initialize()
{
	return S_OK;
}

HRESULT CUI_Manager::Add_UI(const wstring& strAddUITag, CMyUI* pMyUI, void* pDesc)
{
	if(pDesc == nullptr)
		return E_FAIL;

	CMyUI* pUI = Find_UI(strAddUITag);

	if (nullptr != pUI)
	{
		return E_FAIL;
	}

	if(pUI == nullptr)
		return E_FAIL;

	m_UIs.emplace(strAddUITag, pUI);

	return S_OK;
}

HRESULT CUI_Manager::Erase_UI(const wstring& strUITag)
{
	CMyUI* pEraseUI = Find_UI(strUITag);

	auto& iter = m_UIs.find(strUITag);


	m_UIs.erase(iter);

	Safe_Release(pEraseUI);

	

	return S_OK;
}

void CUI_Manager::Tick(_float fTimeDelta)
{
// 	vector<wstring> vecEraseUIs;
// 
// 	for (auto& Pair : m_UIs)
// 	{
// 		Pair.second->Tick(fTimeDelta);
// 
// 		if (true == Pair.second->IsEnd())
// 		{
// 			vecEraseUIs.push_back(Pair.first);
// 		}
// 	}
// 
// 	for(wstring EraseTag : vecEraseUIs)
// 		Erase_UI(EraseTag);
}


HRESULT CUI_Manager::Render_UIs()
{
	for (auto& Pair : m_UIs)
	{
		Pair.second->Render();
	}

	return S_OK;
}


CMyUI* CUI_Manager::Find_UI(const wstring& strUITag)
{
	auto	iter = m_UIs.find(strUITag);

	if (iter == m_UIs.end())
		return nullptr;

	return iter->second;
	
}

CUI_Manager* CUI_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Manager* pInstance = new CUI_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CTarget_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Manager::Free()
{
	Safe_Release(m_pGameInstance);

	for (auto& Pair : m_UIs)
	{
		Safe_Release(Pair.second);
	}

	m_UIs.clear();


	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
