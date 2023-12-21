#include "PlayerState_Idle.h"

#include "Model.h"
#include "Player.h"
#include "PlayerPart_Body.h"
#include "GameInstance.h"

CPlayerState_Idle::CPlayerState_Idle()
{
}

HRESULT CPlayerState_Idle::Initialize(CPlayer* pPlayer)
{
	m_isGround = true;
	m_iAnimIndex = 73;

	m_eStateType = CStateMachine::STATE_GROUND;

	m_strName = TEXT("PlayerState_Idle");

	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;

	m_pOwnerModelCom->Set_Animation(m_iAnimIndex);



	return S_OK;
}

HRESULT CPlayerState_Idle::Priority_Tick(const _float& fTimeDelta)
{
	return S_OK;
}

HRESULT CPlayerState_Idle::Tick(const _float& fTimeDelta)
{
	KeyInput(fTimeDelta);

	return S_OK;
}

HRESULT CPlayerState_Idle::Late_Tick(const _float& fTimeDelta)
{
	return S_OK;
}

HRESULT CPlayerState_Idle::Transition()
{
	
	
	return S_OK;
}

void CPlayerState_Idle::KeyInput(const _float& fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_W))
	{
		m_pPlayer->Set_CurrentState(TEXT("PlayerState_Walk"));
	}

	if (m_pGameInstance->Key_Down(DIK_A))
	{
		m_pPlayer->Set_CurrentState(TEXT("PlayerState_Walk"));
	}

	if (m_pGameInstance->Key_Down(DIK_S))
	{
		m_pPlayer->Set_CurrentState(TEXT("PlayerState_Walk"));
	}

	if (m_pGameInstance->Key_Down(DIK_D))
	{
		m_pPlayer->Set_CurrentState(TEXT("PlayerState_Walk"));
	}

	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		m_pPlayer->Set_CurrentState(TEXT("PlayerState_Jump"));	
	}
}

void CPlayerState_Idle::ResetState()
{
	m_fLastInputTime = 0.f;
	m_fAccTime = 0.f;
}

CPlayerState_Idle* CPlayerState_Idle::Create(CPlayer* pPlayer)
{
	CPlayerState_Idle* pInstance = new CPlayerState_Idle();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_Idle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_Idle::Free()
{
	__super::Free();
}
