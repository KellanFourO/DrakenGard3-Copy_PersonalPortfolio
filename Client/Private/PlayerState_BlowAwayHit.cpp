#include "stdafx.h"
#include "Player.h"
#include "PlayerState_BlowAwayHit.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Camera_Target.h"

CPlayerState_BlowAwayHit::CPlayerState_BlowAwayHit()
{
}

HRESULT CPlayerState_BlowAwayHit::Initialize(CPlayer* pPlayer)
{
	if (FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_BlowAwayHit::StartState()
{
	m_pOwnerModelCom->Set_Animation(49);
	m_pOwnerModelCom->Set_Loop(false);
	m_pOwnerModelCom->Root_MotionStart();
	m_pOwnerStateCom->Set_Hit(true);
	

	return S_OK;
}

HRESULT CPlayerState_BlowAwayHit::EndState()
{
	m_pOwnerModelCom->Set_Loop(true);
	m_bKeyInput = false;
	m_pOwnerRigidBody->Clear_NetPower();
	m_pOwnerStateCom->Set_Hit(false);
	for (_int i = 0; i < 3; ++i)
	{
		m_bCurrentHitAnimEnd[i] = false;
	}
	

	m_pOwnerModelCom->Root_MotionEnd();
	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;

	return S_OK;
}

void CPlayerState_BlowAwayHit::Tick(const _float& fTimeDelta)
{
	
	if (false == Is_Current_AnimEnd() && m_pGameInstance->Key_Down(DIK_SPACE))
	{
		m_bKeyInput = true;
	}

	if (true == Is_Current_AnimEnd() && false == m_bCurrentHitAnimEnd[0])
	{
		m_bCurrentHitAnimEnd[0] = true;

		if (true == m_bKeyInput)
		{
			Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_BreakFall"));
			m_pOwnerRigidBody->Clear_NetPower();
		}
		else
			m_pOwnerModelCom->Set_Animation(44);
	}
	else if (true == Is_Current_AnimEnd() && false == m_bCurrentHitAnimEnd[1])
	{
		m_bCurrentHitAnimEnd[1] = true;

		
			m_pOwnerModelCom->Set_Animation(45);
	}
	
	else if (true == Is_Current_AnimEnd() && false == m_bCurrentHitAnimEnd[2])
	{
		m_bCurrentHitAnimEnd[2] = true;

		m_pOwnerModelCom->Set_Animation(46);
	}

	else if (true == Is_Current_AnimEnd() && true == m_bCurrentHitAnimEnd[2])
	{
		Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Idle"));
	}

	


}



CPlayerState_BlowAwayHit* CPlayerState_BlowAwayHit::Create(CPlayer* pPlayer)
{
	CPlayerState_BlowAwayHit* pInstance = new CPlayerState_BlowAwayHit();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_BlowAwayHit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_BlowAwayHit::Free()
{
	__super::Free();
}
