#include "stdafx.h"
#include "Player.h"
#include "PlayerState_Dead.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Camera_Target.h"

CPlayerState_Dead::CPlayerState_Dead()
{
}

HRESULT CPlayerState_Dead::Initialize(CPlayer* pPlayer)
{
	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_Dead::StartState()
{
	m_pOwnerModelCom->Set_Animation(73);
	m_pOwnerModelCom->Set_Loop(true);
	m_pOwnerModelCom->Root_MotionEnd();
	return S_OK;
}

HRESULT CPlayerState_Dead::EndState()
{
	
	
	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;

	return S_OK;
}

void CPlayerState_Dead::Tick(const _float& fTimeDelta)
{
}



CPlayerState_Dead* CPlayerState_Dead::Create(CPlayer* pPlayer)
{
	CPlayerState_Dead* pInstance = new CPlayerState_Dead();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_Dead");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_Dead::Free()
{
	__super::Free();
}
