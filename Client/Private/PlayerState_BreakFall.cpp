#include "stdafx.h"
#include "Player.h"
#include "PlayerState_BreakFall.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Camera_Target.h"

CPlayerState_BreakFall::CPlayerState_BreakFall()
{
}

HRESULT CPlayerState_BreakFall::Initialize(CPlayer* pPlayer)
{
	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_BreakFall::StartState()
{
	m_pOwnerModelCom->Set_Animation(52);
	m_pOwnerModelCom->Set_Loop(false);
	m_pOwnerModelCom->Root_MotionEnd();
	return S_OK;
}

HRESULT CPlayerState_BreakFall::EndState()
{
	
	m_pOwnerModelCom->Set_Loop(true);
	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;

	return S_OK;
}

void CPlayerState_BreakFall::Tick(const _float& fTimeDelta)
{
	if (true == Is_Current_AnimEnd())
	{
		Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Idle"));
	}
}



CPlayerState_BreakFall* CPlayerState_BreakFall::Create(CPlayer* pPlayer)
{
	CPlayerState_BreakFall* pInstance = new CPlayerState_BreakFall();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_BreakFall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_BreakFall::Free()
{
	__super::Free();
}
