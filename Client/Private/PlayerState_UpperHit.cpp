#include "stdafx.h"
#include "Player.h"
#include "PlayerState_UpperHit.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Camera_Target.h"

CPlayerState_UpperHit::CPlayerState_UpperHit()
{
}

HRESULT CPlayerState_UpperHit::Initialize(CPlayer* pPlayer)
{
	if (FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_UpperHit::StartState()
{
	m_pOwnerModelCom->Set_Animation(37);
	m_pOwnerModelCom->Set_Loop(false);
	m_pOwnerModelCom->Root_MotionStart();
	return S_OK;
}

HRESULT CPlayerState_UpperHit::EndState()
{
	m_pOwnerModelCom->Set_Loop(true);
	m_bKeyInput = false;

	for (_int i = 0; i < 4; ++i)
	{
		m_bCurrentHitAnimEnd[i] = false;
	}
	

	m_pOwnerModelCom->Root_MotionEnd();
	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;

	return S_OK;
}

void CPlayerState_UpperHit::Tick(const _float& fTimeDelta)
{
	
	if (false == Is_Current_AnimEnd() && m_pGameInstance->Key_Down(DIK_SPACE))
	{
		m_bKeyInput = true;
	}

	if (true == Is_Current_AnimEnd() && false == m_bCurrentHitAnimEnd[0])
	{
		m_bCurrentHitAnimEnd[0] = true;

 		
			m_pOwnerModelCom->Set_Animation(38);
	}
	else if (true == Is_Current_AnimEnd() && false == m_bCurrentHitAnimEnd[1])
	{
			m_bCurrentHitAnimEnd[1] = true;

			if (true == m_bKeyInput)
				Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_BreakFall"));
			else
			m_pOwnerModelCom->Set_Animation(39);
	}
	
	else if (true == Is_Current_AnimEnd() && false == m_bCurrentHitAnimEnd[2])
	{
		m_bCurrentHitAnimEnd[2] = true;

		m_pOwnerModelCom->Set_Animation(44);
	}
	else if (true == Is_Current_AnimEnd() && false == m_bCurrentHitAnimEnd[3])
	{
		m_bCurrentHitAnimEnd[3] = true;

		m_pOwnerModelCom->Set_Animation(45);
	}

	else if (true == Is_Current_AnimEnd() && true == m_bCurrentHitAnimEnd[3])
	{
		Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Idle"));
	}

}



CPlayerState_UpperHit* CPlayerState_UpperHit::Create(CPlayer* pPlayer)
{
	CPlayerState_UpperHit* pInstance = new CPlayerState_UpperHit();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_UpperHit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_UpperHit::Free()
{
	__super::Free();
}
