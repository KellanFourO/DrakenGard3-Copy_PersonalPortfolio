#include "stdafx.h"
#include "Player.h"
#include "PlayerState_Attack2.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"

CPlayerState_Attack2::CPlayerState_Attack2()
{
}

HRESULT CPlayerState_Attack2::Initialize(CPlayer* pPlayer)
{
	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_Attack2::StartState()
{
	//m_pOwnerModelCom->Set_Animation(92);
	//m_fDuration = m_pOwnerModelCom->Get_CurrentDuration();
	return S_OK;
}

HRESULT CPlayerState_Attack2::EndState()
{

	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;
	m_bChange = false;
	m_fDuration = 0.f;
	return S_OK;
}

void CPlayerState_Attack2::Tick(const _float& fTimeDelta)
{
	
}

void CPlayerState_Attack2::Late_Tick(const _float& fTimeDelta)
{
	m_fAccTime += 30 * fTimeDelta;

	if (m_fDuration < m_fAccTime && !m_bChange)
	{
		m_pOwnerModelCom->Set_Animation(93);
		m_bChange = true;
		m_fAccTime = 0;
	}

	//if (m_bChange)
	//{
	//	m_fDuration = m_pOwnerModelCom->Get_CurrentDuration();
	//
	//	if (m_fDuration < m_fAccTime)
	//	{
	//		KeyInput(fTimeDelta);
	//
	//		//m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Idle"));
	//	}
	//}
}

void CPlayerState_Attack2::KeyInput(const _float& fTimeDelta)
{
	if (m_pGameInstance->Mouse_Down(DIM_LB))
	{
		m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Attack3"));
	}
}

CPlayerState_Attack2* CPlayerState_Attack2::Create(CPlayer* pPlayer)
{
	CPlayerState_Attack2* pInstance = new CPlayerState_Attack2();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_Attack2");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_Attack2::Free()
{
	__super::Free();
}
