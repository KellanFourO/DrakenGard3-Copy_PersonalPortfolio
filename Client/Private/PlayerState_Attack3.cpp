#include "stdafx.h"
#include "Player.h"
#include "PlayerState_Attack3.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Animation.h"

CPlayerState_Attack3::CPlayerState_Attack3()
{
}

HRESULT CPlayerState_Attack3::Initialize(CPlayer* pPlayer)
{
	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_Attack3::StartState()
{
	m_pOwnerModelCom->Set_Animation(94);
	m_pOwnerModelCom->Set_Loop(false);
	return S_OK;
}

HRESULT CPlayerState_Attack3::EndState()
{

	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;
	m_isEnd = false;
	m_bInput = false;
	return S_OK;
}

void CPlayerState_Attack3::Tick(const _float& fTimeDelta)
{

}

void CPlayerState_Attack3::Late_Tick(const _float& fTimeDelta)
{
	NextComboOrIdle(m_pOwnerModelCom, m_pOwnerStateCom, TEXT("PlayerState_Attack4"), 95);
}

CPlayerState_Attack3* CPlayerState_Attack3::Create(CPlayer* pPlayer)
{
	CPlayerState_Attack3* pInstance = new CPlayerState_Attack3();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_Attack3");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_Attack3::Free()
{
	__super::Free();
}
