#pragma once
#include "StateBase.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CStateMachine;
class CTransform;
class CNavigation;
class CRigidBody;
END

BEGIN(Client)
class CPlayer;


class CPlayerState_Base abstract : public CStateBase
{
protected:
	CPlayerState_Base();
	virtual ~CPlayerState_Base() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer); //! 최초 생성시 셋팅 해야하는 값을 정의하자
	virtual HRESULT StartState() = 0; //! 다시 자기 자신이 호출 될 때의 행동을 정의하자
	virtual HRESULT EndState() = 0;

	virtual void	Tick(const _float& fTimeDelta) = 0;

protected:
	virtual void	KeyInput(const _float& fTimeDelta);

protected:
	CStateMachine*	m_pOwnerStateCom = { nullptr };
	CTransform*		m_pOwnerTransform = { nullptr };
	CNavigation*	m_pOwnerNavagation = { nullptr };
	CRigidBody*		m_pOwnerRigidBody = { nullptr };

protected:
	_float			m_fLastInputTime = { 0.0f };
	_float			m_fEndTime = 0.0f;
	_float			m_fAccTime = 0.0f;

public:
	virtual void Free() override;


};

END