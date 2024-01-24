#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)		
class CPlayer;

class CPlayerState_BreakFall final : public CPlayerState_Base 
{
private:
		 CPlayerState_BreakFall();
virtual ~CPlayerState_BreakFall() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer) override;
	virtual HRESULT StartState() override;
	virtual HRESULT EndState() override;

	virtual void	Tick(const _float& fTimeDelta) override;

private:
	_bool			m_bLookAtCamLook = false;


public:
	static CPlayerState_BreakFall* Create(CPlayer* pPlayer);
	virtual void Free() override;
};

END

