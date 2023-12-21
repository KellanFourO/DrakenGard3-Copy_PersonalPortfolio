#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CModel;
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CStateMachine abstract : public CBase
{
public:
	enum STATETYPE { STATE_GROUND, STATE_AIR, STATE_DEAD, STATE_NONE,  STATE_END };

protected:
		 CStateMachine();
virtual ~CStateMachine() = default;

public:
	_int			Get_AnimIndex() { return m_iAnimIndex; }
	STATETYPE		Get_StateType() { return m_eStateType; }
	wstring			Get_Name() { return m_strName; }

	void			Set_CompulsionUse() { m_isUsing = false; } //! 혹시나 하는 상황에 대비해 강제로 사용가능한 상태로 만들어버리는 함수
	void			Set_CompulsionDead() { m_isDead = false; } //! 마찬가지로 강제로 데드상태로 만들어버리는 함수
	void			Set_CompulsionChangeAnim(_int iIndex) { m_pOwnerModelCom->Set_Animation(iIndex); }

public:
	virtual HRESULT Initialize();
	virtual HRESULT Priority_Tick(const _float& fTimeDelta);
	virtual HRESULT Tick(const _float& fTimeDelta);
	virtual HRESULT Late_Tick(const _float& fTimeDelta);
	virtual HRESULT Transition();

public:
	HRESULT			Replaceability(CStateMachine* pNextState); //! 현재 상태에서 다음 상태로 교체가 가능한지의 대한 여부를 체크하는 함수

protected:
	CModel*			m_pOwnerModelCom = { nullptr };
	CGameInstance*	m_pGameInstance = { nullptr };

protected:
	wstring		m_strName;
	STATETYPE	m_eStateType = { STATE_NONE };
	_int		m_iAnimIndex = { 9999 };

protected:
	_bool		m_isGround = { true };
	_bool		m_isDead = { false };
	_bool		m_isUsing = { false };

public:
	virtual void Free() override;
};

END
