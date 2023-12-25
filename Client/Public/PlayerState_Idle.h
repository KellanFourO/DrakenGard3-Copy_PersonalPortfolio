#pragma once
#include "PlayerState_Base.h"

//BEGIN(Engine)
//class CStateMachine;
//END

BEGIN(Client)

class CPlayerState_Idle final : public CPlayerState_Base 
{
private:
		 CPlayerState_Idle();
virtual ~CPlayerState_Idle() = default;

public:
	virtual HRESULT Initialize(CStateMachine* pStateMachine) override;
	virtual HRESULT StartState() override;
	virtual HRESULT EndState() override;

	virtual void	Tick(const _float& fTimeDelta) override;

private:
	virtual void	KeyInput(const _float& fTimeDelta) override;

public:
	static CPlayerState_Idle* Create(CStateMachine* pStateMachine);
	virtual void Free() override;
};

END

