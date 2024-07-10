#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)		
class CPlayer;

class CPlayerState_UpperHit final : public CPlayerState_Base 
{
private:
		 CPlayerState_UpperHit();
virtual ~CPlayerState_UpperHit() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer) override;
	virtual HRESULT StartState() override;
	virtual HRESULT EndState() override;

	virtual void	Tick(const _float& fTimeDelta) override;

public:
	void			Set_ImpulsePos(_float3 vImpulsePos) { m_vImpulsePos = vImpulsePos; }

private:
	_bool			m_bCurrentHitAnimEnd[4] = { false, false, false, false};
	_bool			m_bKeyInput = false;
	_float3			m_vImpulsePos = {};

public:
	static CPlayerState_UpperHit* Create(CPlayer* pPlayer);
	virtual void Free() override;
};

END

