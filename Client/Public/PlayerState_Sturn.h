#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)		
class CPlayer;

class CPlayerState_Sturn final : public CPlayerState_Base 
{
private:
		 CPlayerState_Sturn();
virtual ~CPlayerState_Sturn() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer) override;
	virtual HRESULT StartState() override;
	virtual HRESULT EndState() override;

	virtual void	Tick(const _float& fTimeDelta) override;


public:
	static CPlayerState_Sturn* Create(CPlayer* pPlayer);
	virtual void Free() override;

private:
	_bool		m_bSturn[2] = { false, false };
};

END

