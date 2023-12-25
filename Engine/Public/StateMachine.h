#pragma once
#include "Component.h"
#include "StateBase.h"

BEGIN(Engine)

class ENGINE_DLL CStateMachine final : public CComponent
{
public:
	enum STATETYPE { STATE_GROUND, STATE_AIR, STATE_DEAD, STATE_NONE,  STATE_END };

public:
	STATETYPE Get_StateType() { return m_eCurrentStateType; }
	void	  Set_StateType(STATETYPE eStateType) { m_eCurrentStateType = eStateType; }

	void	  Set_AnimIndex(_int iAnimIndex) { m_iCurrentAnimIndex = iAnimIndex; }

	wstring	  Get_StateTag() { return m_strCurrentStateTag; }
	void	  Set_StateTag(const wstring& strStateTag) { m_strCurrentStateTag = strStateTag; }
	
	void	  Set_State(CStateMachine* pNextState);

private:
	CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStateMachine(const CStateMachine& rhs);
	virtual	~CStateMachine() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;


private:
	HRESULT			Add_State(const wstring& strStateTag, CStateBase* pAddState);
	
	HRESULT			Find_Exist(const wstring& strStateTag);

private:
	HRESULT			Replaceability(CStateMachine* pNextState); //! 현재 상태에서 다음 상태로 교체가 가능한지의 대한 여부를 체크하는 함수
	
//private:
//	class CGameObject* m_pOwner = { nullptr };

private:
	wstring			m_strCurrentStateTag;
	_int			m_iCurrentAnimIndex = { 0 };
	STATETYPE		m_eCurrentStateType = { STATE_END };

private:
	CStateBase*		m_pCurrentState = { nullptr };
	_bool			m_isGround = { true };
	_bool			m_isFinished = { true };
	_bool			m_isDead = { false };

private:
	map<const wstring, CStateBase*>		m_States;

public:
	static	CStateMachine*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END

