#pragma once
#include "Component.h"
#include "StateBase.h"

BEGIN(Engine)
class CGameObject;
class CModel;

class ENGINE_DLL CStateMachine final : public CComponent
{
public:
	enum STATETYPE { STATE_GROUND, STATE_AIR, STATE_DEAD, STATE_NONE,  STATE_END };

public:
	CGameObject*	Get_Owner() { return m_pOwner;}

	STATETYPE		Get_StateType() { return m_eCurrentStateType; }
	void			Set_StateType(STATETYPE eStateType) { m_eCurrentStateType = eStateType; }

	void			Set_AnimIndex(_int iAnimIndex) { m_iCurrentAnimIndex = iAnimIndex;}

	wstring			Get_StateTag() { return m_strCurrentStateTag; }
	void			Set_StateTag(const wstring& strStateTag) { m_strCurrentStateTag = strStateTag;}

	HRESULT			Add_State(const wstring& strStateTag, CStateBase* pAddState);
	HRESULT			Set_InitState(const wstring& strStateTag);
	HRESULT			Transition(STATETYPE eStateType, const wstring& strStateTag);

	void			Set_ActionDelay(_float fDelay) { m_fActionDelay = fDelay;  m_isActionSwitch = false; }
	
	

private:
	CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStateMachine(const CStateMachine& rhs);
	virtual	~CStateMachine() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

	
	HRESULT			Find_Exist(const wstring& strStateTag);

private:
	HRESULT			Replaceability(STATETYPE eStateType); //! 현재 상태에서 다음 상태로 교체가 가능한지의 대한 여부를 체크하는 함수
	
private:
	CGameObject*	m_pOwner = { nullptr };

private:
	wstring			m_strCurrentStateTag;
	_int			m_iCurrentAnimIndex = { 0 };
	STATETYPE		m_eCurrentStateType = { STATE_END };

private:
	CStateBase*		m_pCurrentState = { nullptr }; 
	_bool			m_isGround = { true };
	_bool			m_isFinished = { true };
	_bool			m_isDead = { false };

	_bool			m_isStart = { true } ; //! 최초 한번
	
	_float			m_fActionDelay = { 0.f };
	_bool			m_isActionSwitch = true;

private:
	map<const wstring, CStateBase*>		m_States;

public:
	static	CStateMachine*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END

