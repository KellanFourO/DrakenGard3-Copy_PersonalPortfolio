#include "stdafx.h"
#include "Player.h"
#include "PlayerState_Attack8.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Animation.h"

CPlayerState_Attack8::CPlayerState_Attack8()
{
}

HRESULT CPlayerState_Attack8::Initialize(CPlayer* pPlayer)
{
	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_Attack8::StartState()
{
	m_pOwnerModelCom->Set_Animation(104);
	m_pOwnerModelCom->Set_Loop(false);
	return S_OK;
}

HRESULT CPlayerState_Attack8::EndState()
{

	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;
	m_isEnd = false;
	m_bInput = false;

	m_pOwnerStateCom->Set_ActionDelay(1.f);

	return S_OK;
}

void CPlayerState_Attack8::Tick(const _float& fTimeDelta)
{
}

void CPlayerState_Attack8::Late_Tick(const _float& fTimeDelta)
{
	NextComboOrIdle(m_pOwnerModelCom, m_pOwnerStateCom, TEXT("PlayerState_Idle"), 105);
}

CPlayerState_Attack8* CPlayerState_Attack8::Create(CPlayer* pPlayer)
{
	CPlayerState_Attack8* pInstance = new CPlayerState_Attack8();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_Attack8");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_Attack8::Free()
{
	__super::Free();
}
