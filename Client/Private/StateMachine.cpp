#include "stdafx.h"
#include "StateMachine.h"
#include "GameInstance.h"

CStateMachine::CStateMachine()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CStateMachine::Initialize()
{
	return S_OK;
}

HRESULT CStateMachine::Priority_Tick(const _float& fTimeDelta)
{
	return S_OK;
}

HRESULT CStateMachine::Tick(const _float& fTimeDelta)
{
	return S_OK;
}

HRESULT CStateMachine::Late_Tick(const _float& fTimeDelta)
{
	return S_OK;
}

HRESULT CStateMachine::Transition()
{
	return S_OK;
}

HRESULT CStateMachine::Replaceability(CStateMachine* pNextState)
{
	//TODO 다음상태가 DEAD 타입이었다면
	if (pNextState->Get_StateType() == CStateMachine::STATE_DEAD)
	{
		m_isDead = true;

		return S_OK;
	}


	switch (pNextState->Get_StateType())
	{
		case STATE_GROUND :
		{
			//TODO 다음상태가 GROUND 타입이라면.

			//!현재 상태가 GROUND 타입이고, 사용중이 아니라면 // 언제든지 바뀌어도 되는 상태의 경우에는 m_isUsing은 항상 false 일것.
			if (m_eStateType == CStateMachine::STATE_GROUND && !m_isUsing)
				return S_OK;
			
			//! 현재 상태가 AIR 타입이며,  땅을 밟고있지 않은 상태면 // AIR타입이면서 갑자기 땅으로 찍어내리는 애니메이션이라면 사용이 끝났을 경우에만 교체가 가능하도록 하자
			else if (!m_isGround && m_eStateType == CStateMachine::STATE_AIR && !m_isUsing)	
				return E_FAIL;

			else
				return E_FAIL;

			break;
		}

		case STATE_AIR :
		{
			//TODO 다음상태가 AIR 타입이라면.

			if (m_eStateType == CStateMachine::STATE_AIR && !m_isUsing)
				return S_OK;

			//! 현재 상태가 GROUND 타입이며,  땅을 밟고있는 상태이면서,  사용중이 아니라면 // 언제든지 바뀌어도 되는 상태의 경우에는 m_isUsing은 항상 false 일것.
			else if (m_isGround && m_eStateType == CStateMachine::STATE_GROUND && !m_isUsing)
				return S_OK;

			else
				return E_FAIL;

			break;
		}

	}


	return E_FAIL;
}

void CStateMachine::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pOwnerModelCom);
}
