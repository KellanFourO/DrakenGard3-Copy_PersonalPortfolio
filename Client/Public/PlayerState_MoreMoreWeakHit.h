#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)		
class CPlayer;

class CPlayerState_MoreMoreWeakHit final : public CPlayerState_Base 
{
private:
		 CPlayerState_MoreMoreWeakHit();
virtual ~CPlayerState_MoreMoreWeakHit() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer) override;
	virtual HRESULT StartState() override;
	virtual HRESULT EndState() override;

	virtual void	Tick(const _float& fTimeDelta) override;

private:
	_bool			m_bLookAtCamLook = false;


public:
	static CPlayerState_MoreMoreWeakHit* Create(CPlayer* pPlayer);
	virtual void Free() override;
};

END

