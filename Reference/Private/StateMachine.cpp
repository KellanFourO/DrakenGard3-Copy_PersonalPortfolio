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
	//TODO �������°� DEAD Ÿ���̾��ٸ�
	if (eStateType == CStateMachine::STATE_DEAD)
	{
		m_isDead = true;

		return S_OK;
	}


	switch (eStateType)
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

	if (true == m_isCloned)
	{
		for (auto& Pair : m_States)
			Safe_Release(Pair.second);

		m_States.clear();
	}
	

	
}