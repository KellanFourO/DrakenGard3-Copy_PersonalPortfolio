#include "EN00State_Chase.h"
#include "GameObject.h"
#include "Model.h"
#include "StateMachine.h"

CEN00State_Chase::CEN00State_Chase()
{
}

HRESULT CEN00State_Chase::Initialize(CGameObject* pOwner)
{
	if(FAILED(__super::Initialize(pOwner)))
		return E_FAIL;
	
	m_fSearchRange = 10.f;

	m_pOwner = pOwner;

	if(FAILED(AddRefIfNotNull(m_pOwner)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CEN00State_Chase::StartState()
{
	m_pOwnerModelCom->Set_Animation(0);
	
	return S_OK;
}

HRESULT CEN00State_Chase::EndState()
{
	return S_OK;
}

void CEN00State_Chase::Priority_Tick(const _float& fTimeDelta)
{
}

void CEN00State_Chase::Tick(const _float& fTimeDelta)
{
	if (true == Chase(fTimeDelta))
	{
		m_pOwnerStateCom->Transition(CStateMachine::STATE_GROUND, TEXT("EN00State_Attack1"));
	}

}

void CEN00State_Chase::Late_Tick(const _float& fTimeDelta)
{
}

CEN00State_Chase* CEN00State_Chase::Create(CGameObject* pOwner)
{
	CEN00State_Chase* pInstance = new CEN00State_Chase();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pOwner)))
	{
		MSG_BOX("Failed to Created : CEN00State_Chase");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEN00State_Chase::Free()
{
	__super::Free();

	Safe_Release(m_pOwner);
}
