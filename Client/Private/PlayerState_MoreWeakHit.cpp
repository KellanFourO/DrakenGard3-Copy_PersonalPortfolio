#include "stdafx.h"
#include "Player.h"
#include "PlayerState_MoreWeakHit.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Camera_Target.h"

CPlayerState_MoreWeakHit::CPlayerState_MoreWeakHit()
{
}

HRESULT CPlayerState_MoreWeakHit::Initialize(CPlayer* pPlayer)
{
	if (FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_MoreWeakHit::StartState()
{
	m_pOwnerModelCom->Set_Animation(27);
	m_pOwnerModelCom->Set_Loop(false);
	m_pOwnerModelCom->Root_MotionStart();
	m_pOwnerStateCom->Set_Hit(true);
	return S_OK;
}

HRESULT CPlayerState_MoreWeakHit::EndState()
{
	m_pOwnerModelCom->Set_Loop(true);
	m_pOwnerRigidBody->Clear_NetPower();
	m_pOwnerModelCom->Root_MotionEnd();
	m_pOwnerStateCom->Set_Hit(false);
	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;

	return S_OK;
}

void CPlayerState_MoreWeakHit::Tick(const _float& fTimeDelta)
{
	if (true == Is_Current_AnimEnd())
	{
		Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Idle"));

	}
}



CPlayerState_MoreWeakHit* CPlayerState_MoreWeakHit::Create(CPlayer* pPlayer)
{
	CPlayerState_MoreWeakHit* pInstance = new CPlayerState_MoreWeakHit();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_MoreWeakHit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_MoreWeakHit::Free()
{
	__super::Free();
}
