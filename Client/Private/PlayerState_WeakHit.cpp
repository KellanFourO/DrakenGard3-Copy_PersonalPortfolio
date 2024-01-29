#include "stdafx.h"
#include "Player.h"
#include "PlayerState_WeakHit.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Camera_Target.h"

CPlayerState_WeakHit::CPlayerState_WeakHit()
{
}

HRESULT CPlayerState_WeakHit::Initialize(CPlayer* pPlayer)
{
	if (FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_WeakHit::StartState()
{
	m_pOwnerModelCom->Set_Animation(28);
	m_pOwnerModelCom->Set_Loop(false);
	m_pOwnerModelCom->Root_MotionStart();
	return S_OK;
}

HRESULT CPlayerState_WeakHit::EndState()
{
	m_pOwnerModelCom->Set_Loop(true);

	m_pOwnerModelCom->Root_MotionEnd();
	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;
	m_pOwnerRigidBody->Clear_NetPower();
	return S_OK;
}

void CPlayerState_WeakHit::Tick(const _float& fTimeDelta)
{
	if (true == Is_Current_AnimEnd())
	{
		Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Idle"));
	}
}



CPlayerState_WeakHit* CPlayerState_WeakHit::Create(CPlayer* pPlayer)
{
	CPlayerState_WeakHit* pInstance = new CPlayerState_WeakHit();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_WeakHit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_WeakHit::Free()
{
	__super::Free();
}
