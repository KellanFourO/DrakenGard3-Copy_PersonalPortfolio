#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)

class CPlayerState_Idle final : public CPlayerState_Base 
{
private:
		 CPlayerState_Idle();
virtual ~CPlayerState_Idle() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer);
	virtual HRESULT Priority_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Tick(const _float& fTimeDelta) override;
	virtual HRESULT Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Transition() override;

private:
	virtual void   KeyInput(const _float& fTimeDelta) override;
	virtual void   ResetState() override;

public:
	static CPlayerState_Idle* Create(CPlayer* pPlayer);
	virtual void Free() override;
};

END

