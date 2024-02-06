#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)		
class CPlayer;

class CPlayerState_BloodyMode_Idle final : public CPlayerState_Base 
{
private:
		 CPlayerState_BloodyMode_Idle();
virtual ~CPlayerState_BloodyMode_Idle() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer) override;
	virtual HRESULT StartState() override;
	virtual HRESULT EndState() override;

	virtual void	Tick(const _float& fTimeDelta) override;

private:
	_bool			m_bLookAtCamLook = false;

private:
	virtual void	KeyInput(const _float& fTimeDelta) override;

public:
	static CPlayerState_BloodyMode_Idle* Create(CPlayer* pPlayer);
	virtual void Free() override;
};

END

