#include "stdafx.h"
#include "AIState_Base.h"
#include "Player.h"
#include "Transform.h"
#include "GameInstance.h"

CAIState_Base::CAIState_Base()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CAIState_Base::Initialize(CGameObject* pOwner)
{
	
	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));
	
	if(FAILED(AddRefIfNotNull(m_pPlayerTransform)))
		return E_FAIL;

	m_pOwnerTransform = dynamic_cast<CTransform*>(pOwner->Find_Component(TEXT("Com_Transform")));
	m_pOwnerStateCom = dynamic_cast<CStateMachine*>(pOwner->Find_Component(TEXT("Com_StateMachine")));
	//!m_pOwnerNavagation = dynamic_cast<CNavigation*>(pOwner->Find_Component(TEXT("Com_Navigation")));
	//m_pOwnerRigidBody = dynamic_cast<CRigidBody*>(pOwner->Find_Component(TEXT("Com_RigidBody")));
	m_pOwnerModelCom = dynamic_cast<CModel*>(pOwner->Find_Component(TEXT("Com_Model")));
	
	if (FAILED(AddRefIfNotNull(m_pOwnerStateCom)))
		return E_FAIL;
	if (FAILED(AddRefIfNotNull(m_pOwnerModelCom)))
		return E_FAIL;
	if (FAILED(AddRefIfNotNull(m_pOwnerTransform)))
		return E_FAIL;
	//if (FAILED(AddRefIfNotNull(m_pOwnerNavagation)))
	//	return E_FAIL;
	//if (FAILED(AddRefIfNotNull(m_pOwnerRigidBody)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CAIState_Base::StartState()
{
	return S_OK;
}

HRESULT CAIState_Base::EndState()
{
	return S_OK;
}

void CAIState_Base::Priority_Tick(const _float& fTimeDelta)
{
}

void CAIState_Base::Tick(const _float& fTimeDelta)
{
}

void CAIState_Base::Late_Tick(const _float& fTimeDelta)
{
}

_bool CAIState_Base::Search()
{
	if(nullptr == m_pOwner || nullptr == m_pPlayerTransform)
		return false;

	CTransform* pOwnerTransform = m_pOwner->Get_Transform();


	_vector vPlayerPos, vOwnerPos;

	vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vOwnerPos = pOwnerTransform->Get_State(CTransform::STATE_POSITION);

	_vector vDistance = XMVector3Length(vPlayerPos - vOwnerPos);

	if (XMVectorGetX(vDistance) < m_fSearchRange)
	{
		return true;
	}

	return false;
}

_bool CAIState_Base::Chase()
{
	if (nullptr == m_pOwner || nullptr == m_pPlayerTransform)
		return false;

	CTransform* pOwnerTransform = m_pOwner->Get_Transform();


	_vector vPlayerPos, vOwnerPos;

	vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vOwnerPos = pOwnerTransform->Get_State(CTransform::STATE_POSITION);

	_vector vDistance = XMVector3Length(vPlayerPos - vOwnerPos);

	if (XMVectorGetX(vDistance) < m_fAttackRange)
	{
		return true;
	}

	return false;
}

void CAIState_Base::Free()
{
	__super::Free();

	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pGameInstance);
}
