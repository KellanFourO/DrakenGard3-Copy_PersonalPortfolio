#include "stdafx.h"
#include "Player.h"
#include "PlayerState_Depress.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Camera_Target.h"

CPlayerState_Depress::CPlayerState_Depress()
{
}

HRESULT CPlayerState_Depress::Initialize(CPlayer* pPlayer)
{
	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_Depress::StartState()
{
	m_pOwnerModelCom->Set_Animation(69);
	m_pOwnerModelCom->Set_Loop(false);
	m_pOwnerModelCom->Root_MotionEnd();
	return S_OK;
}

HRESULT CPlayerState_Depress::EndState()
{
	
	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;
	m_bChangeAnim = false;

	return S_OK;
}

void CPlayerState_Depress::Tick(const _float& fTimeDelta)
{
	if (true == Is_Current_AnimEnd() && false == m_bChangeAnim)
	{
		m_pOwnerModelCom->Set_Loop(true);
		m_pOwnerModelCom->Set_Animation(70);

		m_bChangeAnim = true;
	}


	
	KeyInput(fTimeDelta);


}

void CPlayerState_Depress::KeyInput(const _float& fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		Transition(CStateMachine::STATE_GROUND, TEXT("PlayerState_Idle"));
	}
	
}

CPlayerState_Depress* CPlayerState_Depress::Create(CPlayer* pPlayer)
{
	CPlayerState_Depress* pInstance = new CPlayerState_Depress();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_Depress");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_Depress::Free()
{
	__super::Free();
}
