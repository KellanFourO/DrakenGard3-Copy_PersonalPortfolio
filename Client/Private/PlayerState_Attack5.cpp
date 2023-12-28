#include "stdafx.h"
#include "Player.h"
#include "PlayerState_Attack5.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Animation.h"

CPlayerState_Attack5::CPlayerState_Attack5()
{
}

HRESULT CPlayerState_Attack5::Initialize(CPlayer* pPlayer)
{
	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_Attack5::StartState()
{
	m_pOwnerModelCom->Set_Animation(98);
	m_pOwnerModelCom->Set_Loop(false);
	m_pOwnerModelCom->Root_MotionChange();
	return S_OK;
}

HRESULT CPlayerState_Attack5::EndState()
{

	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;
	m_isEnd = false;
	m_bInput = false;
	m_pOwnerModelCom->Reset_RootMotion();

	return S_OK;
}

void CPlayerState_Attack5::Priority_Tick(const _float& fTimeDelta)
{
	if (false == m_isEnd)
	{
		RootMotion();
	}
}

void CPlayerState_Attack5::Tick(const _float& fTimeDelta)
{
}

void CPlayerState_Attack5::Late_Tick(const _float& fTimeDelta)
{
	NextComboOrIdle(m_pOwnerModelCom, m_pOwnerStateCom, TEXT("PlayerState_Attack6"), 99);
}

CPlayerState_Attack5* CPlayerState_Attack5::Create(CPlayer* pPlayer)
{
	CPlayerState_Attack5* pInstance = new CPlayerState_Attack5();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_Attack5");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_Attack5::Free()
{
	__super::Free();
}
