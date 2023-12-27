#include "stdafx.h"
#include "Player.h"
#include "PlayerState_Idle.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"

CPlayerState_Idle::CPlayerState_Idle()
{
}

HRESULT CPlayerState_Idle::Initialize(CPlayer* pPlayer)
{
	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_Idle::StartState()
{
	m_pOwnerModelCom->Set_Animation(73);
	m_pOwnerModelCom->Set_Loop(true);
	return S_OK;
}

HRESULT CPlayerState_Idle::EndState()
{
	
	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;

	return S_OK;
}

void CPlayerState_Idle::Tick(const _float& fTimeDelta)
{
	KeyInput(fTimeDelta);
}

void CPlayerState_Idle::KeyInput(const _float& fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_W))
	{
		m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Walk"));
			
	}

	if (m_pGameInstance->Key_Down(DIK_A))
	{
		m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Walk"));
	}

	if (m_pGameInstance->Key_Down(DIK_S))
	{
		m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Walk"));
	}

	if (m_pGameInstance->Key_Down(DIK_D))
	{
		m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Walk"));
	}

	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_AIR, TEXT("PlayerState_Jump"));
	}

	if (m_pGameInstance->Mouse_Down(DIM_LB))
	{
		m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Attack1"));
	}
}

CPlayerState_Idle* CPlayerState_Idle::Create(CPlayer* pPlayer)
{
	CPlayerState_Idle* pInstance = new CPlayerState_Idle();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_Idle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_Idle::Free()
{
	__super::Free();
}
