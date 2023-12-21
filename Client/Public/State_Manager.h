#pragma once
#include "Base.h"
#include "Client_Defines.h"

#include "StateMachine.h"

BEGIN(Client)

class CState_Manager final : public CBase
{
	DECLARE_SINGLETON(CState_Manager);

private:
			 CState_Manager();
	virtual ~CState_Manager() = default;
	
public:
	HRESULT			Add_StateMachine(const wstring& strProtoStateTag, CStateMachine* pProtoStateMachine);
	CStateMachine*  Get_CloneState(const wstring& strProtoStateTag);
	CStateMachine*	Find_State(const wstring& strProtoStateTag);

public:
	virtual void Free() override;

private:
	map<const wstring&, CStateMachine*> m_States;

};

END

