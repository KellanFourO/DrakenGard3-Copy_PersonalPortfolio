#include "MyEvent.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Transform.h"
#include "GameObject.h"

CMyEvent::CMyEvent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CMyEvent::Play_Event(EVENTTYPE eEventType)
{
}

HRESULT CMyEvent::Initialize(EVENT_DESC* pDesc)
{
	m_tEventDesc = *pDesc;

	if(FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_tEventDesc.vEventPos));

    return S_OK;
}

void CMyEvent::Tick(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());


	if (false == m_bFindCollider)
	{
		CGameObject* pPlayer = m_pGameInstance->Get_Player(m_tEventDesc.iCurrentLevelIndex);
		
			CCollider* pPlayerCollider = { nullptr };
			pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Find_Component(TEXT("Com_Collider")));

			if (pPlayerCollider != nullptr)
			{
				m_bFindCollider = true;
				m_pTargetCollider = pPlayerCollider;
			}
	}
	
	if (m_pTargetCollider != nullptr && false == m_bStart)
	{
		if (true == m_pColliderCom->Collision(m_pTargetCollider))
		{
			m_bStart = true;
		}
	}

	if(false == m_bEnd && true == m_bStart)
		Play_Event(m_tEventDesc.eEventType);
	
	
}

#ifdef _DEBUG
HRESULT CMyEvent::Render()
{
	m_pColliderCom->Render();
    return S_OK;
}
#endif // _DEBUG

HRESULT CMyEvent::Ready_Components()
{
	
	/* For.Com_Collider */
	CBoundingBox_AABB::BOUNDING_AABB_DESC		BoundingDesc = {};

	BoundingDesc.vExtents = m_tEventDesc.vColliderSize;
	BoundingDesc.vCenter = m_tEventDesc.vColliderCenter;
	

	m_pColliderCom = dynamic_cast<CCollider*>(m_pGameInstance->Clone_Component(m_tEventDesc.iCurrentLevelIndex, TEXT("Prototype_Component_Collider_AABB"), &BoundingDesc));
	
	if(nullptr == m_pColliderCom)
		return E_FAIL;

	Safe_AddRef(m_pColliderCom);

	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext, 0.F, 0.f);

	if(nullptr == m_pTransformCom)
		return E_FAIL;

	Safe_AddRef(m_pTransformCom);

    return S_OK;
}





// CMyEvent* CMyEvent::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, EVENT_DESC* pDesc)
// {
// 	CMyEvent* pInstance = new CMyEvent(pDevice, pContext);
// 
// 	/* 원형객체를 초기화한다.  */
// 	if (FAILED(pInstance->Initialize(pDesc)))
// 	{
// 		MSG_BOX("Failed to Created : CMyEvent");
// 		Safe_Release(pInstance);
// 	}
// 	return pInstance;
// }

void CMyEvent::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
