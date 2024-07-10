#include "Layer.h"
#include "GameObject.h"
#include "PartObject.h"

CLayer::CLayer()
{
}

CComponent* CLayer::Get_Component(const wstring& strComponentTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Find_Component(strComponentTag);
}

CComponent* CLayer::Get_PartComponent(const wstring& strComponentTag, _uint iIndex, const wstring& strPartTag)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Find_PartObject(strPartTag)->Find_Component(strComponentTag);
}

CGameObject* CLayer::Get_Player()
{
	
	for (auto& pGameObject : m_GameObjects)
	{
		if(true == pGameObject->Is_Player())
			return pGameObject;
	}
	
	return nullptr;
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
	if(nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);
	
	return S_OK;
}

HRESULT CLayer::Erase_GameObject(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	auto iter = find(m_GameObjects.begin(), m_GameObjects.end(), pGameObject);

	if (iter == m_GameObjects.end())
		return E_FAIL;

	// 게임 오브젝트를 삭제
	Safe_Release(*iter);

	// 반복자를 무효화하기 전에 삭제 작업을 수행
	m_GameObjects.erase(iter);


	return S_OK;
}

void CLayer::Priority_Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if(nullptr != pGameObject)
			pGameObject->Priority_Tick(fTimeDelta);
	}
}

void CLayer::Tick(_float fTimeDelta)
{
	vector<CGameObject*> vecDeleteObjects;

	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
		{

			pGameObject->Tick(fTimeDelta);

			if (true == pGameObject->Is_Dead())
			{	
				_float fLifeTime = pGameObject->Get_LifeTime();
				
				m_fTimeAcc += fTimeDelta;
				if (m_fTimeAcc >= fLifeTime)
				{
					vecDeleteObjects.push_back(pGameObject);

					m_fTimeAcc = 0.f;
				}
			}
		}
	}

	for (auto& pGameObject : vecDeleteObjects)
	{
		Erase_GameObject(pGameObject);
	}
}

void CLayer::Late_Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Late_Tick(fTimeDelta);
	}
}

CLayer* CLayer::Create()
{
	return new CLayer;
}

void CLayer::Free()
{
	for(auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
}
