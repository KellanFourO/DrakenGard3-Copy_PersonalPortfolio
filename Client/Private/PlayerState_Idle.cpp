#include "stdafx.h"
#include "PlayerState_Idle.h"


CPlayerState_Idle::CPlayerState_Idle()
{
}

HRESULT CPlayerState_Idle::Initialize(CStateMachine* pStateMachine)
{
	//m_pOwnerStateCom = pStateMachine;

	//if(FAILED(AddRefIfNotNull(m_pOwnerStateCom)))
	//	return E_FAIL;
	

	return S_OK;
}

HRESULT CPlayerState_Idle::StartState()
{
	//pStateMachine->Set_StateTag(TEXT("PlayerState_Idle"));
	//pStateMachine->Set_StateType(CStateMachine::STATE_GROUND);
	//
	//pStateMachine->Set_AnimIndex(73);


	return S_OK;
}

HRESULT CPlayerState_Idle::EndState()
{
	return S_OK;
}

void CPlayerState_Idle::Tick(const _float& fTimeDelta)
{
	
}

void CPlayerState_Idle::KeyInput(const _float& fTimeDelta)
{
	
}

CPlayerState_Idle* CPlayerState_Idle::Create(CStateMachine* pStateMachine)
{
	CPlayerState_Idle* pInstance = new CPlayerState_Idle();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pStateMachine)))
	{
		MSG_BOX("Failed to Created : CPlayerState_Idle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_Idle::Free()
{
}
