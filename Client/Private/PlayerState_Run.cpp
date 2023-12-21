#include "PlayerState_Run.h"

#include "Model.h"
#include "Transform.h"
#include "Player.h"
#include "PlayerPart_Body.h"
#include "GameInstance.h"

CPlayerState_Run::CPlayerState_Run()
{
}

HRESULT CPlayerState_Run::Initialize(CPlayer* pPlayer)
{
	m_isGround = true;
	m_iAnimIndex = 76;
	m_fEndTime = 0.25f;

	m_eStateType = CStateMachine::STATE_GROUND;

	m_strName = TEXT("PlayerState_Run");

	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;

	m_pOwnerModelCom->Set_Animation(m_iAnimIndex);

	m_pOwnerTransformCom->Set_SpeedPerSec(4.f);
	m_pOwnerTransformCom->Set_RotationPerSec(XMConvertToRadians(90.0f));

	return S_OK;
}

HRESULT CPlayerState_Run::Priority_Tick(const _float& fTimeDelta)
{
	return S_OK;
}

HRESULT CPlayerState_Run::Tick(const _float& fTimeDelta)
{
	KeyInput(fTimeDelta);

	return S_OK;
}

HRESULT CPlayerState_Run::Late_Tick(const _float& fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_W) || m_pGameInstance->Key_Pressing(DIK_A)
		|| m_pGameInstance->Key_Pressing(DIK_S) || m_pGameInstance->Key_Pressing(DIK_D))
		m_bKeyPressing = true;
	else
		m_bKeyPressing = false;

	m_fAccTime += m_fLastInputTime + fTimeDelta;

	if (m_fAccTime > m_fEndTime && !m_bKeyPressing)
		m_pPlayer->Set_CurrentState(TEXT("PlayerState_Idle"));

	return S_OK;
}

HRESULT CPlayerState_Run::Transition()
{
	
	
	return S_OK;
}

void CPlayerState_Run::KeyInput(const _float& fTimeDelta)
{
	

	if (m_pGameInstance->Key_Pressing(DIK_W))
	{
		m_pOwnerTransformCom->Go_Straight(fTimeDelta, m_pOwnerNavigationCom);
		m_fLastInputTime = fTimeDelta;
	}

	if (m_pGameInstance->Key_Pressing(DIK_A))
	{
		m_pOwnerTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
		m_fLastInputTime = fTimeDelta;
	}

	if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		m_pOwnerTransformCom->Go_Backward(fTimeDelta);
		m_fLastInputTime = fTimeDelta;
	}

	if (m_pGameInstance->Key_Pressing(DIK_D))
	{
		m_pOwnerTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 1.f);
		m_fLastInputTime = fTimeDelta;
	}

	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		m_pPlayer->Set_CurrentState(TEXT("PlayerState_Jump"));
	}
	

	
}

void CPlayerState_Run::ResetState()
{
	m_fLastInputTime = 0.f;
	m_fAccTime = 0.f;
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
