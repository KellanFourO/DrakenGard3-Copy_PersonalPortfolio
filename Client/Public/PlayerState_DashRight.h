#pragma once
#include "PlayerState_Base.h"


BEGIN(Client)
class CPlayer;

class CPlayerState_DashRight final : public CPlayerState_Base
{
private:
	CPlayerState_DashRight();
	virtual ~CPlayerState_DashRight() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer) override;
	virtual HRESULT StartState() override;
	virtual HRESULT EndState() override;

	virtual void	Tick(const _float& fTimeDelta) override;
	virtual void	Late_Tick(const _float& fTimeDelta) override;


private:
	_bool			m_bKeyPressing = false;


public:
	static CPlayerState_DashRight* Create(CPlayer* pPlayer);
	virtual void Free() override;
};

END

