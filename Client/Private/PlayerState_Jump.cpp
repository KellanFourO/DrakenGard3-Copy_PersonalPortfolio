#include "stdafx.h"
#include "Player.h"
#include "PlayerState_Jump.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"

CPlayerState_Jump::CPlayerState_Jump()
{
}

HRESULT CPlayerState_Jump::Initialize(CPlayer* pPlayer)
{
	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerState_Jump::StartState()
{

	return S_OK;
}

HRESULT CPlayerState_Jump::EndState()
{
	return S_OK;
}

void CPlayerState_Jump::Tick(const _float& fTimeDelta)
{
	
}

void CPlayerState_Jump::KeyInput(const _float& fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_W))
	{
		//m_pOwnerStateCom->Set_State()
		//m_pPlayer->Set_CurrentState(TEXT("PlayerState_Walk"));
	}

	if (m_pGameInstance->Key_Down(DIK_A))
	{
		//m_pPlayer->Set_CurrentState(TEXT("PlayerState_Walk"));
	}

	if (m_pGameInstance->Key_Down(DIK_S))
	{
		//m_pPlayer->Set_CurrentState(TEXT("PlayerState_Walk"));
	}

	if (m_pGameInstance->Key_Down(DIK_D))
	{
		//m_pPlayer->Set_CurrentState(TEXT("PlayerState_Walk"));
	}

	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		//m_pPlayer->Set_CurrentState(TEXT("PlayerState_Jump"));
	}
}

CPlayerState_Jump* CPlayerState_Jump::Create(CPlayer* pPlayer)
{
	CPlayerState_Jump* pInstance = new CPlayerState_Jump();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_Jump");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_Jump::Free()
{
	__super::Free();
}
