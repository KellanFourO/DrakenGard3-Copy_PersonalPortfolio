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
#include "Animation.h"

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
	__super::StartState();
	m_pOwnerModelCom->Set_Animation(92);
	m_pOwnerModelCom->Set_Loop(false);
	m_pOwnerModelCom->Root_MotionStart();

	return S_OK;
}

HRESULT CPlayerState_Attack2::EndState()
{
	__super::EndState();
	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;
	m_isEnd = false;
	m_bInput = false;

	return S_OK;
}

void CPlayerState_Attack2::Priority_Tick(const _float& fTimeDelta)
{
	
}

void CPlayerState_Attack2::Tick(const _float& fTimeDelta)
{
}

void CPlayerState_Attack2::Late_Tick(const _float& fTimeDelta)
{
	NextComboOrIdle(m_pOwnerModelCom, m_pOwnerStateCom, TEXT("PlayerState_Attack3"), 93);
	
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
