#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)
class CPlayer;

class CPlayerState_Attack3 final : public CPlayerState_Base 
{
private:
		 CPlayerState_Attack3();
virtual ~CPlayerState_Attack3() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer) override;
	virtual HRESULT StartState() override;
	virtual HRESULT EndState() override;

	virtual void	Tick(const _float& fTimeDelta) override;
	virtual void	Late_Tick(const _float& fTimeDelta) override;

private:
	virtual void	KeyInput(const _float& fTimeDelta) override;

private:
	_bool			m_bChange = false;
	_float			m_fDuration = 0.f;

public:
	static CPlayerState_Attack3* Create(CPlayer* pPlayer);
	virtual void Free() override;
};

END

