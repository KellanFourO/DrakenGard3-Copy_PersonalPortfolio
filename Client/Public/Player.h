#pragma once

#include "Client_Defines.h"
#include "AnimObject.h"
#include "StateMachine.h"

BEGIN(Engine)
class CCollider;
class CNavigation;
END

BEGIN(Client)

class CPartObject;


class CPlayer final : public CAnimObject
{

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;


public:
	HRESULT			Set_CurrentState(const wstring& strStateTag);
	

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CPartObject*	Find_PartObject(const wstring& strPartTag);
	CStateMachine*	Find_States(const wstring& strStateTag);

private:
	HRESULT			Ready_Components();
	HRESULT			Ready_PartObjects();
	HRESULT			Ready_States();
	HRESULT			Add_PartObject(const wstring& strPrototypeTag, const wstring& strPartTag, void* pArg);
	HRESULT			Add_State(const wstring& strStateTag, class CStateMachine* pStateMachine);
	void			Key_Input(const _float fTimeDelta);
	void			Admin_SetAnimIndex(_int iAnimIndex) { m_pCurrentState->Set_CompulsionChangeAnim(iAnimIndex); };

private:
	CNavigation*		m_pNavigationCom = { nullptr };
	CCollider*			m_pColliderCom = { nullptr };
	CStateMachine*		m_pCurrentState = { nullptr };

private:
	_int				m_iCurrentAnimIndex = { 0 };
	_bool				m_bAdmin = false;
	

private:
	map<const wstring, class CPartObject*>			m_PartObjects;
	map<const wstring, class CStateMachine*>		m_States;

public:
	/* 원형객체를 생성한다. */
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END