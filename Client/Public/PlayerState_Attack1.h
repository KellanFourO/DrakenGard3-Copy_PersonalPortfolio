#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayer;

class CPlayerState_Attack1 final : public CPlayerState_Base 
{
private:
		 CPlayerState_Attack1();
virtual ~CPlayerState_Attack1() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer) override;
	virtual HRESULT StartState() override;
	virtual HRESULT EndState() override;

	virtual void	Tick(const _float& fTimeDelta) override;
	virtual void	Late_Tick(const _float& fTimeDelta) override;

private:
	virtual void	KeyInput(const _float& fTimeDelta) override;

private:
	_float			m_fDuration = 0.f;
	_bool			m_bChange = false;

public:
	static CPlayerState_Attack1* Create(CPlayer* pPlayer);
	virtual void Free() override;
};

END

