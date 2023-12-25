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
}

HRESULT CStateMachine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStateMachine::Initialize(void* pArg)
{
	return S_OK;
}

void CStateMachine::Set_State(CStateMachine* pNextState)
{
	//!CStateMachine* pStateMachine = Find_States(strStateTag);
	//!
	//!if (nullptr == pStateMachine)
	//!	return E_FAIL;
	//!
	//!
	//!//if(nullptr != m_pCurrentState && FAILED(m_pCurrentState->Replaceability(pStateMachine)))
	//!//	return S_OK;
	//!
	//!if (nullptr != m_pCurrentState)
	//!	static_cast<CPlayerState_Base*>(m_pCurrentState)->ResetState();
	//!
	//!m_pCurrentState = pStateMachine;
	//!
	//!CPlayerPart_Body* pBody = static_cast<CPlayerPart_Body*>(Find_PartObject(TEXT("Part_Body")));
	//!Safe_AddRef(pBody);
	//!
	//!pBody->SetUp_Animation(m_pCurrentState->Get_AnimIndex());
	//!
	//!Safe_Release(pBody);
	//!
	//!return S_OK;
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
		case STATE_GROUND:
		{
			//TODO �������°� GROUND Ÿ���̶��.

			//!���� ���°� GROUND Ÿ���̰�, ������� �ƴ϶�� // �������� �ٲ� �Ǵ� ������ ��쿡�� m_isUsing�� �׻� false �ϰ�.
			if (m_eCurrentStateType == CStateMachine::STATE_GROUND && !m_isFinished)
				return S_OK;

			//! ���� ���°� AIR Ÿ���̸�,  ���� ������� ���� ���¸� // AIRŸ���̸鼭 ���ڱ� ������ ������ �ִϸ��̼��̶�� ����� ������ ��쿡�� ��ü�� �����ϵ��� ����
			else if (!m_isGround && m_eCurrentStateType == CStateMachine::STATE_AIR && !m_isFinished)
				return E_FAIL;

			else
				return E_FAIL;

			break;
		}

		case STATE_AIR:
		{
			//TODO �������°� AIR Ÿ���̶��.

			if (m_eCurrentStateType == CStateMachine::STATE_AIR && !m_isFinished)
				return S_OK;

			//! ���� ���°� GROUND Ÿ���̸�,  ���� ����ִ� �����̸鼭,  ������� �ƴ϶�� // �������� �ٲ� �Ǵ� ������ ��쿡�� m_isUsing�� �׻� false �ϰ�.
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

HRESULT CStateMachine::Find_Exist(const wstring& strStateTag)
{
	auto iter = m_States.find(strStateTag);

	//TODO ���� �̸��� ���� ���°� �����ٸ� �������� �ʾҴ�.

	if(iter == m_States.end())
		return S_OK;
	
	return E_FAIL;
}

CStateMachine* CStateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStateMachine* pInstance = new CStateMachine(pDevice, pContext);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
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

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
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
}