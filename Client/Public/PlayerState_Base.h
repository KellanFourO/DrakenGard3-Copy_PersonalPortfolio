#pragma once
#include "StateMachine.h"

BEGIN(Engine)
class CNavigation;
class CTransform;
END

BEGIN(Client)

class CPlayer;

class CPlayerState_Base abstract : public CStateMachine 
{
protected:
		 CPlayerState_Base();
virtual ~CPlayerState_Base() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer);
	virtual HRESULT Priority_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Tick(const _float& fTimeDelta) override;
	virtual HRESULT Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Transition() override;
	virtual void	ResetState();

protected:
	virtual	void	KeyInput(const _float& fTimeDelta);

protected:
	CPlayer*		m_pPlayer = { nullptr };
	CNavigation*	m_pOwnerNavigationCom = { nullptr };
	CTransform*		m_pOwnerTransformCom = { nullptr };

protected:
	_float			m_fLastInputTime = { 0.0f };
	_float			m_fEndTime = 0.0f;
	_float			m_fAccTime = 0.0f;

public:	
	virtual void Free() override;


};

END
