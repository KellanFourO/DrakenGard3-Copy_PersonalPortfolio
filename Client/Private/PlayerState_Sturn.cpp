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
	m_pOwnerModelCom->Set_Animation(73);
	m_pOwnerModelCom->Set_Loop(true);
	m_pOwnerModelCom->Root_MotionEnd();
	return S_OK;
}

HRESULT CPlayerState_Sturn::EndState()
{
	
	
	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;

	return S_OK;
}

void CPlayerState_Sturn::Tick(const _float& fTimeDelta)
{
	KeyInput(fTimeDelta);
}

void CPlayerState_Sturn::KeyInput(const _float& fTimeDelta)
{
	
	//_float3 vCamLook;
	//XMStoreFloat3(&vCamLook, m_pOwnerCam->Get_Transform()->Get_State(CTransform::STATE_LOOK));
	//m_pOwnerTransform->Look_At_CamLook(vCamLook);

	if (m_pGameInstance->Key_Down(DIK_W))
	{
		m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Walk"));
			
	}

	if (m_pGameInstance->Key_Down(DIK_A))
	{
		m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Walk"));
	}

	if (m_pGameInstance->Key_Down(DIK_S))
	{
		m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Walk"));
	}

	if (m_pGameInstance->Key_Down(DIK_D))
	{
		m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Walk"));
	}

	//if (m_pGameInstance->Key_Down(DIK_SPACE))
	//{
	//	m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_AIR, TEXT("PlayerState_Jump"));
	//}

	if (m_pGameInstance->Mouse_Down(DIM_LB))
	{
		m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Attack1"));
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
