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
	//TODO �������°� DEAD Ÿ���̾��ٸ�
	if (pNextState->Get_StateType() == CStateMachine::STATE_DEAD)
	{
		m_isDead = true;

		return S_OK;
	}


	switch (pNextState->Get_StateType())
	{
		case STATE_GROUND :
		{
			//TODO �������°� GROUND Ÿ���̶��.

			//!���� ���°� GROUND Ÿ���̰�, ������� �ƴ϶�� // �������� �ٲ� �Ǵ� ������ ��쿡�� m_isUsing�� �׻� false �ϰ�.
			if (m_eStateType == CStateMachine::STATE_GROUND && !m_isUsing)
				return S_OK;
			
			//! ���� ���°� AIR Ÿ���̸�,  ���� ������� ���� ���¸� // AIRŸ���̸鼭 ���ڱ� ������ ������ �ִϸ��̼��̶�� ����� ������ ��쿡�� ��ü�� �����ϵ��� ����
			else if (!m_isGround && m_eStateType == CStateMachine::STATE_AIR && !m_isUsing)	
				return E_FAIL;

			else
				return E_FAIL;

			break;
		}

		case STATE_AIR :
		{
			//TODO �������°� AIR Ÿ���̶��.

			if (m_eStateType == CStateMachine::STATE_AIR && !m_isUsing)
				return S_OK;

			//! ���� ���°� GROUND Ÿ���̸�,  ���� ����ִ� �����̸鼭,  ������� �ƴ϶�� // �������� �ٲ� �Ǵ� ������ ��쿡�� m_isUsing�� �׻� false �ϰ�.
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
