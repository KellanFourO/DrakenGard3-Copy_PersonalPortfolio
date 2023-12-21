#include "PlayerState_Jump.h"

#include "Model.h"
#include "Player.h"
#include "PlayerPart_Body.h"
#include "GameInstance.h"

CPlayerState_Jump::CPlayerState_Jump()
{
}

HRESULT CPlayerState_Jump::Initialize(CPlayer* pPlayer)
{
	m_isGround = false;
	m_iAnimIndex = 0;
	m_fEndTime = 0.5f; //TODO 추후 수정

	m_eStateType = CStateMachine::STATE_AIR;

	m_strName = TEXT("PlayerState_Jump");

	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;

	m_pOwnerModelCom->Set_Animation(m_iAnimIndex);

	return S_OK;
}

HRESULT CPlayerState_Jump::Priority_Tick(const _float& fTimeDelta)
{
	return S_OK;
}

HRESULT CPlayerState_Jump::Tick(const _float& fTimeDelta)
{

	return S_OK;
}

HRESULT CPlayerState_Jump::Late_Tick(const _float& fTimeDelta)
{
	return S_OK;
}

HRESULT CPlayerState_Jump::Transition()
{
	
	
	return S_OK;
}

void CPlayerState_Jump::KeyInput(const _float& fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		
		m_pPlayer->Set_CurrentState(TEXT("PlayerState_Jump"));	
	}

	//TODO 점프 후 자신의 점프 최대거리가 닿은 후 땅과 닿을때까지 추락 모션 으로 바뀔것
	//! 추락 중 어떠한 키입력을 받아 다른 상태로 처리할 것.

}

void CPlayerState_Jump::ResetState()
{
	m_fLastInputTime = 0.f;
	m_fAccTime = 0.f;
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
