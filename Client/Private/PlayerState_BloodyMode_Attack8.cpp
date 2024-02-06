#include "stdafx.h"
#include "Player.h"
#include "PlayerState_BloodyMode_Attack8.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Animation.h"
#include "Effect_Hanabira.h"
#include "Effect_Trail.h"

CPlayerState_BloodyMode_Attack8::CPlayerState_BloodyMode_Attack8()
{
}

HRESULT CPlayerState_BloodyMode_Attack8::Initialize(CPlayer* pPlayer)
{
	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_BloodyMode_Attack8::StartState()
{
	m_pOwnerColliderCom->OnCollider();
	m_pOwnerModelCom->Set_AnimationSpeed(1.75f);
	m_pOwnerModelCom->Set_Animation(485);
	m_pOwnerModelCom->Set_Loop(false);
	m_pOwnerModelCom->Root_MotionStart();
	m_pOwnerTrail->Reset_Points();
	m_pOwnerTrail->On_Trail();
	m_bBloodyMode = true;

	return S_OK;
}

HRESULT CPlayerState_BloodyMode_Attack8::EndState()
{
	__super::EndState();
	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;
	m_isEnd = false;
	m_bInput = false;
	m_bCreateHanabira = false;

	return S_OK;
}

void CPlayerState_BloodyMode_Attack8::Priority_Tick(const _float& fTimeDelta)
{

}

void CPlayerState_BloodyMode_Attack8::Tick(const _float& fTimeDelta)
{
// 	if (18 < m_pOwnerModelCom->Get_CurrentAnimation()->Get_TrackPosition() && false == m_bCreateHanabira)
// 	{
// 		CreateHanabira();
// 
// 
// 		m_pGameInstance->Play_Sound(L"PLAYER_EFFECT", L"SwordFinal1.wav", SOUND_EFFECT2, 2.f);
// 		
// 		
// 		m_bCreateHanabira = true;
// 	}
// 
// 	if (34 < m_pOwnerModelCom->Get_CurrentAnimation()->Get_TrackPosition() && false == m_bPlaySound)
// 	{
// 		m_pGameInstance->Play_Sound(L"PLAYER_EFFECT", L"SwordFinal2.wav", SOUND_EFFECT2, 2.f);
// 
// 		m_bPlaySound = true;
// 	}
}

void CPlayerState_BloodyMode_Attack8::Late_Tick(const _float& fTimeDelta)
{
	NextComboOrIdle(m_pOwnerModelCom, m_pOwnerStateCom, TEXT("PlayerState_BloodyMode_Idle"), 485);
}

void CPlayerState_BloodyMode_Attack8::CreateHanabira()
{
	
		

		for (_int i = 0; i < 300; ++i)
		{
			
			_vector vMyPos = m_pOwnerTransform->Get_State(CTransform::STATE_POSITION);
			_float4 vRandomPos = m_pOwnerTransform->Get_RandomPositionAroundCenter(vMyPos, XMConvertToRadians(360.f));

			CEffect_Hanabira::HANABIRA_DESC Desc;

			Desc.fSpeedPerSec = 5.f;
			Desc.fRotationPerSec = XMConvertToRadians(90.f);
			Desc.pTarget = m_pGameInstance->Get_Player(LEVEL_GAMEPLAY);
			Desc.vPos = vRandomPos;
			Desc.fLifeTime = 1.f;
			Desc.eType = CEffect_Hanabira::HANABIRA_ORBIT;
			XMStoreFloat4(&Desc.vLook, m_pOwnerTransform->Get_State(CTransform::STATE_LOOK));

			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Hanabira"), &Desc);
		}


}

CPlayerState_BloodyMode_Attack8* CPlayerState_BloodyMode_Attack8::Create(CPlayer* pPlayer)
{
	CPlayerState_BloodyMode_Attack8* pInstance = new CPlayerState_BloodyMode_Attack8();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_BloodyMode_Attack8");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_BloodyMode_Attack8::Free()
{
	__super::Free();
}
