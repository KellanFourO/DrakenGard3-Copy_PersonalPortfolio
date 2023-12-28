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
	m_pOwnerModelCom->Set_Animation(92);
	m_pOwnerModelCom->Set_Loop(false);

	//! 루트 모션이 필요한 애니메이션이다. 모델에게 루트모션을 시작하기 위한 이전 위치의 루트 본 위치를 구하라고 하자.
	m_pOwnerModelCom->Root_MotionChange();
	//m_pOwnerModelCom->Root_MotionStart();
	return S_OK;
}

HRESULT CPlayerState_Attack2::EndState()
{

	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;
	m_isEnd = false;
	m_bInput = false;
	m_pOwnerModelCom->Reset_RootMotion();
	//! 현재 콤보공격 애니메이션이 종료 되었다. 루트 모션을 종료하자.
	
	return S_OK;
}

void CPlayerState_Attack2::Priority_Tick(const _float& fTimeDelta)
{
	if (false == m_isEnd)
	{
		RootMotion();
	}
}

void CPlayerState_Attack2::Tick(const _float& fTimeDelta)
{
	//RootMotion();
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
