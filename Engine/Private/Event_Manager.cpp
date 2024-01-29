#include "..\Public\Event_Manager.h"
#include "GameInstance.h"
#include "MyEvent.h"

CEvent_Manager::CEvent_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CEvent_Manager::Initialize()
{
	return S_OK;
}

HRESULT CEvent_Manager::Add_Event(const wstring& strAddEventTag, CMyEvent* pMyEvent, void* pDesc)
{
	if(pDesc == nullptr)
		return E_FAIL;

	CMyEvent* pEvent = Find_Event(strAddEventTag);

	if (nullptr != pEvent)
	{
		return E_FAIL;
	}

	if(pMyEvent == nullptr)
		return E_FAIL;

	m_Events.emplace(strAddEventTag, pMyEvent);

	return S_OK;
}

HRESULT CEvent_Manager::Erase_Event(const wstring& strEraseEventTag)
{
	CMyEvent* pEraseEvent = Find_Event(strEraseEventTag);

	auto& iter = m_Events.find(strEraseEventTag);


	m_Events.erase(iter);

	Safe_Release(pEraseEvent);

	

	return S_OK;
}

void CEvent_Manager::Tick(_float fTimeDelta)
{
	vector<wstring> vecEraseEvents;

	for (auto& Pair : m_Events)
	{
		Pair.second->Tick(fTimeDelta);

		if (true == Pair.second->IsEnd())
		{
			vecEraseEvents.push_back(Pair.first);
		}
	}

	for(wstring EraseTag : vecEraseEvents)
		Erase_Event(EraseTag);
}

HRESULT CEvent_Manager::Render_Events()
{
	for (auto& Pair : m_Events)
	{
		Pair.second->Render();
	}

	return S_OK;
}

CMyEvent* CEvent_Manager::Find_Event(const wstring& strEventTag)
{
	auto	iter = m_Events.find(strEventTag);

	if (iter == m_Events.end())
		return nullptr;

	return iter->second;
	
}

CEvent_Manager* CEvent_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEvent_Manager* pInstance = new CEvent_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CTarget_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Manager::Free()
{
	Safe_Release(m_pGameInstance);

	for (auto& Pair : m_Events)
	{
		Safe_Release(Pair.second);
	}

	m_Events.clear();


	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
