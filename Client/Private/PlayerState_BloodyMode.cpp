#include "stdafx.h"
#include "Player.h"
#include "PlayerState_BloodyMode.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Camera_Target.h"

CPlayerState_BloodyMode::CPlayerState_BloodyMode()
{
}

HRESULT CPlayerState_BloodyMode::Initialize(CPlayer* pPlayer)
{
	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_BloodyMode::StartState()
{
	m_pOwnerModelCom->Set_Animation(152);
	m_pOwnerModelCom->Set_Loop(false);
	m_pOwnerModelCom->Root_MotionEnd();
	return S_OK;
}

HRESULT CPlayerState_BloodyMode::EndState()
{
	
	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;

	return S_OK;
}

void CPlayerState_BloodyMode::Tick(const _float& fTimeDelta)
{
	if (true == Is_Current_AnimEnd())
	{
		Transition(CStateMachine::STATE_GROUND, TEXT("PlayerState_BloodyMode_Idle"));
	}


	
	//KeyInput(fTimeDelta);


}

void CPlayerState_BloodyMode::KeyInput(const _float& fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		m_pGameInstance->Play_BGM(L"BGM", L"BOSSBGM2.wav", 1.f);
		Transition(CStateMachine::STATE_GROUND, TEXT("PlayerState_BloodyMode_Idle"));
	}
	
}

CPlayerState_BloodyMode* CPlayerState_BloodyMode::Create(CPlayer* pPlayer)
{
	CPlayerState_BloodyMode* pInstance = new CPlayerState_BloodyMode();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_BloodyMode");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_BloodyMode::Free()
{
	__super::Free();
}
