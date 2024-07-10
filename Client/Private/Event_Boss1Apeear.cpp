#include "stdafx.h"
#include "Event_Boss1Apeear.h"
#include "Boss_EN131.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Player.h"
#include "Transform.h"
#include "Camera_Target.h"

CEvent_Boss1Appear::CEvent_Boss1Appear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMyEvent(pDevice, pContext)
{
}

HRESULT CEvent_Boss1Appear::Initialize(EVENT_DESC* pDesc)
{
	if(FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CEvent_Boss1Appear::Tick(_float fTimeDelta)
{	
	if(false == m_bFindObject)
	{
		m_pBoss = dynamic_cast<CBoss_EN131*>(m_pGameInstance->Find_Layer(m_tEventDesc.iCurrentLevelIndex, TEXT("Layer_Boss"))->Get_ObjectList().back());
		m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Player(m_tEventDesc.iCurrentLevelIndex));
		m_bFindObject = true;

	
	}


	__super::Tick(fTimeDelta);
}

#ifdef _DEBUG
HRESULT CEvent_Boss1Appear::Render()
{
	__super::Render();

	return S_OK;
}
#endif // _DEBUG


void CEvent_Boss1Appear::Play_Event(EVENTTYPE eEventType)
{
	if (true == m_bFindObject)
	{
		if (m_bStopSound == false)
		{
			m_pGameInstance->Stop_Sound(SOUND_BGM);
			m_bStopSound = true;
		}

		CCamera_Target* pTargetCamera = dynamic_cast<CCamera_Target*>(m_pGameInstance->Find_Layer(m_tEventDesc.iCurrentLevelIndex, TEXT("Layer_Camera"))->Get_ObjectList().back());

		CCamera_Target::CUTSCENE_DESC Desc;

		Desc.fChaseSpeed = 130.f;
		Desc.fStopRange = 10.f;
		Desc.pChaseTarget = m_pBoss;
		XMStoreFloat3(&Desc.vStartPos, m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		Desc.vStartPos.y += 5.f;
		XMStoreFloat3(&Desc.vChasePos, Desc.pChaseTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		
		
		pTargetCamera->Set_OffSet(Desc.pChaseTarget->Get_Offset());
		
		pTargetCamera->Set_CutSceneDesc(&Desc);

		m_pBoss->Start_Appear();

		EndEvent();
	}
	
}

HRESULT CEvent_Boss1Appear::Ready_Components()
{
	if(FAILED(__super::Ready_Components()))
		return E_FAIL;
	
	return S_OK;
}

CEvent_Boss1Appear* CEvent_Boss1Appear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, EVENT_DESC* pDesc)
{
	CEvent_Boss1Appear* pInstance = new CEvent_Boss1Appear(pDevice, pContext);
	 
	 	/* 원형객체를 초기화한다.  */
	 	if (FAILED(pInstance->Initialize(pDesc)))
	 	{
	 		MSG_BOX("Failed to Created : CEvent_Boss1Appear");
	 		Safe_Release(pInstance);
	 	}
	 	return pInstance;
}

void CEvent_Boss1Appear::Free()
{
	__super::Free();
}
