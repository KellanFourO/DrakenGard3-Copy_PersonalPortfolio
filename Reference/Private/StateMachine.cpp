#include "StateMachine.h"
#include "GameInstance.h"
#include "GameObject.h"

CStateMachine::CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CStateMachine::CStateMachine(const CStateMachine& rhs)
	: CComponent(rhs)
{
	m_isCloned = true;
}

HRESULT CStateMachine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStateMachine::Initialize(void* pArg)
{
	CGameObject* pOwner = static_cast<CGameObject*>(pArg);
	
	if(nullptr == pOwner)
		return E_FAIL;
	
	m_pOwner = pOwner;
	
	AddRefIfNotNull(m_pOwner);


	return S_OK;
}

void CStateMachine::Priority_Tick(_float fTimeDelta)
{	
	if (nullptr != m_pCurrentState)
	{
		m_pCurrentState->Priority_Tick(fTimeDelta);
	}
}

void CStateMachine::Tick(_float fTimeDelta)
{
	if (nullptr != m_pCurrentState)
	{
		m_pCurrentState->Tick(fTimeDelta);
	}
}

void CStateMachine::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pCurrentState)
	{
		m_pCurrentState->Late_Tick(fTimeDelta);
	}

	
}


HRESULT CStateMachine::Transition(STATETYPE eStateType, const wstring& strStateTag)
{

	auto iter = m_States.find(strStateTag);

	if(iter == m_States.end())
		return E_FAIL;

	//if(FAILED(Replaceability(eStateType)))
	//	return E_FAIL;
	if(iter->second == nullptr)
		return E_FAIL;

	m_pCurrentState->EndState();

	m_pCurrentState = iter->second;
	m_strCurrentStateTag = strStateTag;
	m_eCurrentStateType = eStateType;
	m_pCurrentState->StartState();
}


HRESULT CStateMachine::Replaceability(STATETYPE eStateType)
{
	//TODO 다음상태가 DEAD 타입이었다면
	if (eStateType == CStateMachine::STATE_DEAD)
	{
		m_isDead = true;

		return S_OK;
	}


	switch (eStateType)
	{
		case STATE_GROUND:
		{
			//TODO 다음상태가 GROUND 타입이라면.

			//!현재 상태가 GROUND 타입이고, 사용중이 아니라면 // 언제든지 바뀌어도 되는 상태의 경우에는 m_isUsing은 항상 false 일것.
			if (m_eCurrentStateType == CStateMachine::STATE_GROUND && !m_isFinished)
				return S_OK;

			//! 현재 상태가 AIR 타입이며,  땅을 밟고있지 않은 상태면 // AIR타입이면서 갑자기 땅으로 찍어내리는 애니메이션이라면 사용이 끝났을 경우에만 교체가 가능하도록 하자
			else if (!m_isGround && m_eCurrentStateType == CStateMachine::STATE_AIR && !m_isFinished)
				return E_FAIL;

			else
				return E_FAIL;

			break;
		}

		case STATE_AIR:
		{
			//TODO 다음상태가 AIR 타입이라면.

			if (m_eCurrentStateType == CStateMachine::STATE_AIR && !m_isFinished)
				return S_OK;

			//! 현재 상태가 GROUND 타입이며,  땅을 밟고있는 상태이면서,  사용중이 아니라면 // 언제든지 바뀌어도 되는 상태의 경우에는 m_isUsing은 항상 false 일것.
			else if (m_isGround && m_eCurrentStateType == CStateMachine::STATE_GROUND && !m_isFinished)
				return S_OK;

			else
				return E_FAIL;

			break;
		}

	}
}

HRESULT CStateMachine::Add_State(const wstring& strStateTag, CStateBase* pAddState)
{
	if(FAILED(Find_Exist(strStateTag)))
		return E_FAIL;

	m_States.emplace(strStateTag, pAddState);

	return S_OK;
}

HRESULT CStateMachine::Set_InitState(const wstring& strStateTag)
{
	auto iter = m_States.find(strStateTag);

	if(iter == m_States.end())
		return E_FAIL;

	m_pCurrentState = iter->second;
	
	return S_OK;
}

HRESULT CStateMachine::Find_Exist(const wstring& strStateTag)
{
	auto iter = m_States.find(strStateTag);

	//TODO 같은 이름을 가진 상태가 없었다면 존재하지 않았다.

	if(iter == m_States.end())
		return S_OK;
	
	return E_FAIL;
}

CStateMachine* CStateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStateMachine* pInstance = new CStateMachine(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStateMachine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CStateMachine::Clone(void* pArg)
{
	CStateMachine* pInstance = new CStateMachine(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStateMachine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStateMachine::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		for (auto& Pair : m_States)
			Safe_Release(Pair.second);

		m_States.clear();
	}
	

	
}