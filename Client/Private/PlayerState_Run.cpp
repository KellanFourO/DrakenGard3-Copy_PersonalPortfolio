#include "stdafx.h"
#include "Player.h"
#include "PlayerState_Run.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"

CPlayerState_Run::CPlayerState_Run()
{
}

HRESULT CPlayerState_Run::Initialize(CPlayer* pPlayer)
{
	if (FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;

	m_fEndTime = 0.25f; //! 일정시간 키 입력 없을시 Idle 상태로 돌아가기.
	

	return S_OK;
}

HRESULT CPlayerState_Run::StartState()
{
	m_pOwnerModelCom->Set_Animation(76);
	m_pOwnerTransform->Set_SpeedPerSec(4.f);
	m_pOwnerTransform->Set_RotationPerSec(XMConvertToRadians(90.0f));

	return S_OK;
}

HRESULT CPlayerState_Run::EndState()
{
	m_fLastInputTime = 0.f;
	m_fAccTime = 0.f;

	return S_OK;
}

void CPlayerState_Run::Tick(const _float& fTimeDelta)
{
	KeyInput(fTimeDelta);
}

void CPlayerState_Run::Late_Tick(const _float& fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_W) || m_pGameInstance->Key_Pressing(DIK_A)
		|| m_pGameInstance->Key_Pressing(DIK_S) || m_pGameInstance->Key_Pressing(DIK_D))
		m_bKeyPressing = true;
	else
		m_bKeyPressing = false;

	m_fAccTime += m_fLastInputTime + fTimeDelta;

	if (m_fAccTime > m_fEndTime && !m_bKeyPressing)
		m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Idle"));
}

void CPlayerState_Run::KeyInput(const _float& fTimeDelta)
{

	if (m_pGameInstance->Key_Pressing(DIK_W))
	{
		m_pOwnerTransform->Go_Straight(fTimeDelta, m_pOwnerNavagation);
		m_fLastInputTime = fTimeDelta;
	}

	if (m_pGameInstance->Key_Pressing(DIK_A))
	{
		m_pOwnerTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
		m_fLastInputTime = fTimeDelta;
	}

	if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		m_pOwnerTransform->Go_Backward(fTimeDelta);
		m_fLastInputTime = fTimeDelta;
	}

	if (m_pGameInstance->Key_Pressing(DIK_D))
	{
		m_pOwnerTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 1.f);
		m_fLastInputTime = fTimeDelta;
	}

	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_AIR, TEXT("PlayerState_Jump"));
	}

	if (m_pGameInstance->Mouse_Down(DIM_LB))
	{
		m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Attack1"));
	}
}

CPlayerState_Run* CPlayerState_Run::Create(CPlayer* pPlayer)
{
	CPlayerState_Run* pInstance = new CPlayerState_Run();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_Run");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_Run::Free()
{
	__super::Free();
}
