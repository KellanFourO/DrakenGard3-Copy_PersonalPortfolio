#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayer;

class CPlayerState_Attack2_End final : public CPlayerState_Base 
{
private:
		 CPlayerState_Attack2_End();
virtual ~CPlayerState_Attack2_End() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer) override;
	virtual HRESULT StartState() override;
	virtual HRESULT EndState() override;

	virtual void	Tick(const _float& fTimeDelta) override;
	virtual void	Late_Tick(const _float& fTimeDelta) override;

private:
	virtual void	KeyInput(const _float& fTimeDelta) override;

public:
	static CPlayerState_Attack2_End* Create(CPlayer* pPlayer);
	virtual void Free() override;
};

END

