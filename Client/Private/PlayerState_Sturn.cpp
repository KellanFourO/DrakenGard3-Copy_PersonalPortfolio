#include "stdafx.h"
#include "Player.h"
#include "PlayerState_Sturn.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Camera_Target.h"

CPlayerState_Sturn::CPlayerState_Sturn()
{
}

HRESULT CPlayerState_Sturn::Initialize(CPlayer* pPlayer)
{
	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_Sturn::StartState()
{
	m_pOwnerModelCom->Set_Animation(29);
	m_pOwnerModelCom->Set_Loop(false);
	m_pOwnerModelCom->Root_MotionEnd();
	return S_OK;
}

HRESULT CPlayerState_Sturn::EndState()
{
	m_bSturn[0] = false;
	m_bSturn[1] = false;
	m_pOwnerModelCom->Set_Loop(true);
	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;

	return S_OK;
}

void CPlayerState_Sturn::Tick(const _float& fTimeDelta)
{
	if (true == Is_Current_AnimEnd() && false == m_bSturn[0])
	{
		m_pOwnerModelCom->Set_Animation(30);
		m_bSturn[0] = true;
	}
	else if (true == Is_Current_AnimEnd() && false == m_bSturn[1])
	{
		m_pOwnerModelCom->Set_Animation(31);
		m_bSturn[1] = true;
	}

	if (true == m_bSturn[1])
	{
		Transition(CStateMachine::STATE_GROUND, TEXT("PlayerState_Idle"));
	}
}


CPlayerState_Sturn* CPlayerState_Sturn::Create(CPlayer* pPlayer)
{
	CPlayerState_Sturn* pInstance = new CPlayerState_Sturn();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_Sturn");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_Sturn::Free()
{
	__super::Free();
}
