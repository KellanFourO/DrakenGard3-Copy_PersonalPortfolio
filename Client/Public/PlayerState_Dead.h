#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)		
class CPlayer;

class CPlayerState_Dead final : public CPlayerState_Base 
{
private:
		 CPlayerState_Dead();
virtual ~CPlayerState_Dead() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer) override;
	virtual HRESULT StartState() override;
	virtual HRESULT EndState() override;

	virtual void	Tick(const _float& fTimeDelta) override;

private:
	_bool			m_bLookAtCamLook = false;

public:
	static CPlayerState_Dead* Create(CPlayer* pPlayer);
	virtual void Free() override;
};

END

