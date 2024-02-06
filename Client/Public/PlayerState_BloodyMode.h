#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)		
class CPlayer;

class CPlayerState_BloodyMode final : public CPlayerState_Base 
{
private:
		 CPlayerState_BloodyMode();
virtual ~CPlayerState_BloodyMode() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer) override;
	virtual HRESULT StartState() override;
	virtual HRESULT EndState() override;

	virtual void	Tick(const _float& fTimeDelta) override;

private:
	virtual void	KeyInput(const _float& fTimeDelta) override;

public:
	static CPlayerState_BloodyMode* Create(CPlayer* pPlayer);
	virtual void Free() override;

private:
	_bool		m_bChangeAnim = false;
};

END

