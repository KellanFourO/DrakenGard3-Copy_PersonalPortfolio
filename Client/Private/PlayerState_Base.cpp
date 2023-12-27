#include "stdafx.h"

#include "Player.h"
#include "PlayerState_Base.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "Bone.h"
#include "Model.h"
#include "Animation.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Navigation.h"
#include "StateMachine.h"


CPlayerState_Base::CPlayerState_Base()
{
}

HRESULT CPlayerState_Base::Initialize(CPlayer* pPlayer)
{
	
	m_pOwnerTransform = dynamic_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	m_pOwnerStateCom =	dynamic_cast<CStateMachine*>(pPlayer->Find_Component(TEXT("Com_StateMachine")));
	m_pOwnerNavagation = dynamic_cast<CNavigation*>(pPlayer->Find_Component(TEXT("Com_Navigation")));
	m_pOwnerRigidBody = dynamic_cast<CRigidBody*>(pPlayer->Find_Component(TEXT("Com_RigidBody")));
	m_pOwnerModelCom = dynamic_cast<CModel*>(pPlayer->Find_PartObject(TEXT("Part_Body"))->Find_Component(TEXT("Com_Model")));

	if(__super::Initialize(m_pOwnerModelCom))
		return E_FAIL;
	if (FAILED(AddRefIfNotNull(m_pOwnerStateCom)))
		return E_FAIL;
	if (FAILED(AddRefIfNotNull(m_pOwnerTransform)))
		return E_FAIL;
	if (FAILED(AddRefIfNotNull(m_pOwnerNavagation)))
		return E_FAIL;
	if (FAILED(AddRefIfNotNull(m_pOwnerRigidBody)))
		return E_FAIL;
	
	return S_OK;
}

void CPlayerState_Base::KeyInput(const _float& fTimeDelta)
{
}

void CPlayerState_Base::RootMotion()
{
	m_pOwnerModelCom->Root_Motion(m_pOwnerTransform);
}

void CPlayerState_Base::Free()
{
	__super::Free();

	Safe_Release(m_pOwnerStateCom);
	Safe_Release(m_pOwnerTransform);
	Safe_Release(m_pOwnerRigidBody);
	Safe_Release(m_pOwnerNavagation);
}
