#pragma once
#include "PlayerState_Base.h"


BEGIN(Client)
class CPlayer;

class CPlayerState_Run final : public CPlayerState_Base 
{
private:
		 CPlayerState_Run();
virtual ~CPlayerState_Run() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer) override;
	virtual HRESULT StartState() override;
	virtual HRESULT EndState() override;

	virtual void	Tick(const _float& fTimeDelta) override;
	virtual void	Late_Tick(const _float& fTimeDelta) override;

private:
	virtual void	KeyInput(const _float& fTimeDelta) override;

private:
	_bool	m_bKeyPressing = false;

public:
	static CPlayerState_Run* Create(CPlayer* pPlayer);
	virtual void Free() override;
};

END

