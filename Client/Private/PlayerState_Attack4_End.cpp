#include "stdafx.h"
#include "Player.h"
#include "PlayerState_Attack4_End.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"

CPlayerState_Attack4_End::CPlayerState_Attack4_End()
{
}

HRESULT CPlayerState_Attack4_End::Initialize(CPlayer* pPlayer)
{
	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_Attack4_End::StartState()
{
	m_pOwnerModelCom->Set_Animation(97);
	m_pOwnerModelCom->Set_Loop(false);
	return S_OK;
}

HRESULT CPlayerState_Attack4_End::EndState()
{
	
	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;
	m_bInput = false;
	return S_OK;
}

void CPlayerState_Attack4_End::Tick(const _float& fTimeDelta)
{
	KeyInput(fTimeDelta);
}

void CPlayerState_Attack4_End::Late_Tick(const _float& fTimeDelta)
{
	

	if (false == m_bInput && true == m_pOwnerModelCom->Get_FinishedAnim())
	{
		m_pOwnerStateCom->Transition(CStateMachine::STATE_GROUND, TEXT("PlayerState_Idle"));
	}
	//!else if (true == m_bInput)
	//!{
	//!	m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Attack5"));
	//!}

}

void CPlayerState_Attack4_End::KeyInput(const _float& fTimeDelta)
{
	if (m_pGameInstance->Mouse_Down(DIM_LB))
	{
		m_bInput = true;
	}
}

CPlayerState_Attack4_End* CPlayerState_Attack4_End::Create(CPlayer* pPlayer)
{
	CPlayerState_Attack4_End* pInstance = new CPlayerState_Attack4_End();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_Attack4_End");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_Attack4_End::Free()
{
	__super::Free();
}
