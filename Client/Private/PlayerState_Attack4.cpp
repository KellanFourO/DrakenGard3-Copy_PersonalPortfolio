#include "stdafx.h"
#include "Player.h"
#include "PlayerState_Attack4.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Animation.h"

CPlayerState_Attack4::CPlayerState_Attack4()
{
}

HRESULT CPlayerState_Attack4::Initialize(CPlayer* pPlayer)
{
	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_Attack4::StartState()
{
	m_pOwnerModelCom->Set_Animation(96);
	m_pOwnerModelCom->Set_Loop(false);
	m_pOwnerModelCom->Root_MotionStart();
	return S_OK;
}

HRESULT CPlayerState_Attack4::EndState()
{

	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;
	m_isEnd = false;
	m_bInput = false;
	m_pOwnerModelCom->Root_MotionEnd();
	return S_OK;
}

void CPlayerState_Attack4::Tick(const _float& fTimeDelta)
{
	RootMotion();
}

void CPlayerState_Attack4::Late_Tick(const _float& fTimeDelta)
{
	NextComboOrIdle(m_pOwnerModelCom, m_pOwnerStateCom, TEXT("PlayerState_Attack5"), 97);
}

CPlayerState_Attack4* CPlayerState_Attack4::Create(CPlayer* pPlayer)
{
	CPlayerState_Attack4* pInstance = new CPlayerState_Attack4();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_Attack4");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_Attack4::Free()
{
	__super::Free();
}
