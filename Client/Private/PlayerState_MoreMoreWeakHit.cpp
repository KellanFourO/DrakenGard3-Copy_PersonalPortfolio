#include "stdafx.h"
#include "Player.h"
#include "PlayerState_MoreMoreWeakHit.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Camera_Target.h"

CPlayerState_MoreMoreWeakHit::CPlayerState_MoreMoreWeakHit()
{
}

HRESULT CPlayerState_MoreMoreWeakHit::Initialize(CPlayer* pPlayer)
{
	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_MoreMoreWeakHit::StartState()
{
	m_pOwnerModelCom->Set_Animation(26);
	m_pOwnerModelCom->Set_Loop(false);
	m_pOwnerModelCom->Root_MotionStart();
	return S_OK;
}

HRESULT CPlayerState_MoreMoreWeakHit::EndState()
{
	m_pOwnerModelCom->Set_Loop(true);
	
	m_pOwnerModelCom->Root_MotionEnd();
	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;

	return S_OK;
}

void CPlayerState_MoreMoreWeakHit::Tick(const _float& fTimeDelta)
{
	if (true == Is_Current_AnimEnd())
	{
		Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Idle"));
	}
}



CPlayerState_MoreMoreWeakHit* CPlayerState_MoreMoreWeakHit::Create(CPlayer* pPlayer)
{
	CPlayerState_MoreMoreWeakHit* pInstance = new CPlayerState_MoreMoreWeakHit();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_MoreMoreWeakHit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_MoreMoreWeakHit::Free()
{
	__super::Free();
}
