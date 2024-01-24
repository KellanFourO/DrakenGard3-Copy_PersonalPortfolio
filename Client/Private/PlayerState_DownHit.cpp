#include "stdafx.h"
#include "Player.h"
#include "PlayerState_DownHit.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Camera_Target.h"

CPlayerState_DownHit::CPlayerState_DownHit()
{
}

HRESULT CPlayerState_DownHit::Initialize(CPlayer* pPlayer)
{
	if (FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_DownHit::StartState()
{
	m_pOwnerModelCom->Set_Animation(40);
	m_pOwnerModelCom->Set_Loop(false);
	m_pOwnerModelCom->Root_MotionStart();
	return S_OK;
}

HRESULT CPlayerState_DownHit::EndState()
{
	m_pOwnerModelCom->Set_Loop(true);
	m_bKeyInput = false;

	m_bCurrentHitAnimEnd = false;
	
	

	m_pOwnerModelCom->Root_MotionEnd();
	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;

	return S_OK;
}

void CPlayerState_DownHit::Tick(const _float& fTimeDelta)
{
	
	if (false == Is_Current_AnimEnd() && m_pGameInstance->Key_Down(DIK_SPACE))
	{
		m_bKeyInput = true;
	}

	if (true == Is_Current_AnimEnd() && false == m_bCurrentHitAnimEnd)
	{
			m_bCurrentHitAnimEnd = true;
 		
			if (true == m_bKeyInput)
				Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_BreakFall"));
			else 
				m_pOwnerModelCom->Set_Animation(46);
	}
	else if (true == Is_Current_AnimEnd() && true == m_bCurrentHitAnimEnd)
	{
		Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Idle"));
	}

	

}



CPlayerState_DownHit* CPlayerState_DownHit::Create(CPlayer* pPlayer)
{
	CPlayerState_DownHit* pInstance = new CPlayerState_DownHit();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_DownHit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_DownHit::Free()
{
	__super::Free();
}
