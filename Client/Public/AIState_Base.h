#pragma once
#include "StateBase.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CGameObject;
class CGameInstance;
class CTransform;
class CStateMachine;
class CNavigation;
class CRigidBody;
END


BEGIN(Client)

class CAIState_Base abstract : public CStateBase
{
protected:
		 CAIState_Base();
virtual ~CAIState_Base() = default;

public:
	virtual HRESULT Initialize(CGameObject* pOwner); //! ���� ������ ���� �ؾ��ϴ� ���� ��������
	virtual HRESULT StartState() override; //! �ٽ� �ڱ� �ڽ��� ȣ�� �� ���� �ൿ�� ��������
	virtual HRESULT EndState() override;   //! �ڱ� �ڽ��� ȣ���� ���� �� ���� �ൿ�� ��������. ex) Reset

	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;

protected:
	_bool	Search();
	_bool	Chase();

protected:
	CGameObject*	m_pOwner = { nullptr };

	CTransform*		m_pPlayerTransform = { nullptr };
	CGameInstance*	m_pGameInstance = { nullptr };

	CStateMachine*	m_pOwnerStateCom = { nullptr };
	CTransform*		m_pOwnerTransform = { nullptr };
	CNavigation*	m_pOwnerNavagation = { nullptr };
	CRigidBody*		m_pOwnerRigidBody = { nullptr };


protected:
	_float			m_fSearchRange = 0.f;
	_float			m_fAttackRange = 0.f;

public:
	virtual void Free();
};

END

