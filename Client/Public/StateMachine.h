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

	void			Set_CompulsionUse() { m_isUsing = false; } //! Ȥ�ó� �ϴ� ��Ȳ�� ����� ������ ��밡���� ���·� ���������� �Լ�
	void			Set_CompulsionDead() { m_isDead = false; } //! ���������� ������ ������·� ���������� �Լ�
	void			Set_CompulsionChangeAnim(_int iIndex) { m_pOwnerModelCom->Set_Animation(iIndex); }

public:
	virtual HRESULT Initialize();
	virtual HRESULT Priority_Tick(const _float& fTimeDelta);
	virtual HRESULT Tick(const _float& fTimeDelta);
	virtual HRESULT Late_Tick(const _float& fTimeDelta);
	virtual HRESULT Transition();

public:
	HRESULT			Replaceability(CStateMachine* pNextState); //! ���� ���¿��� ���� ���·� ��ü�� ���������� ���� ���θ� üũ�ϴ� �Լ�

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
