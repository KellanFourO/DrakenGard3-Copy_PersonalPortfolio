#include "stdafx.h"
#include "Player.h"
#include "PlayerState_BloodyMode_Attack1.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Animation.h"

CPlayerState_BloodyMode_Attack1::CPlayerState_BloodyMode_Attack1()
{
}

HRESULT CPlayerState_BloodyMode_Attack1::Initialize(CPlayer* pPlayer)
{
	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_BloodyMode_Attack1::StartState()
{
	__super::StartState();

	m_pOwnerModelCom->Set_Animation(471);
	m_pOwnerModelCom->Set_Loop(false);
	m_pOwnerModelCom->Root_MotionStart();
	m_bBloodyMode = true;
	
	return S_OK;
}

HRESULT CPlayerState_BloodyMode_Attack1::EndState()
{
	__super::EndState();

	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;
	m_isEnd = false;
	m_bInput = false;


	return S_OK;
}

void CPlayerState_BloodyMode_Attack1::Priority_Tick(const _float& fTimeDelta)
{
	
}

void CPlayerState_BloodyMode_Attack1::Tick(const _float& fTimeDelta)
{
	if (11 < m_pOwnerModelCom->Get_CurrentAnimation()->Get_TrackPosition() && false == m_bPlaySound)
	{
		m_pGameInstance->Play_Sound(L"PLAYER_EFFECT", L"SwordFinal1.wav", SOUND_EFFECT2, 2.f);
		m_bPlaySound = true;	
	}

	

}

void CPlayerState_BloodyMode_Attack1::Late_Tick(const _float& fTimeDelta)
{
	NextComboOrIdle(m_pOwnerModelCom, m_pOwnerStateCom, TEXT("PlayerState_BloodyMode_Attack2"), 472);
	//! 여기서 다음 애니메이션으로 변경되고있다.
}

CPlayerState_BloodyMode_Attack1* CPlayerState_BloodyMode_Attack1::Create(CPlayer* pPlayer)
{
	CPlayerState_BloodyMode_Attack1* pInstance = new CPlayerState_BloodyMode_Attack1();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_BloodyMode_Attack1");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_BloodyMode_Attack1::Free()
{
	__super::Free();
}
