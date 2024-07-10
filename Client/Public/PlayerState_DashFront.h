#pragma once
#include "PlayerState_Base.h"


BEGIN(Client)
class CPlayer;

class CPlayerState_DashFront final : public CPlayerState_Base
{
private:
	CPlayerState_DashFront();
	virtual ~CPlayerState_DashFront() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer) override;
	virtual HRESULT StartState() override;
	virtual HRESULT EndState() override;

	virtual void	Tick(const _float& fTimeDelta) override;
	virtual void	Late_Tick(const _float& fTimeDelta) override;

private:
	void	Create_DashRing();
	void	Create_Hanabira(_float fTimeDelta);

private:
	_bool			m_bKeyPressing = false;

	_float			m_fHanabiraAcc = 0.f;
	_float			m_fCreateHabiraTime = 0.1f;



public:
	static CPlayerState_DashFront* Create(CPlayer* pPlayer);
	virtual void Free() override;
};

END

