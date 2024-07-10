#pragma once
#include "PlayerState_Base.h"


BEGIN(Client)
class CPlayer;

class CPlayerState_BloodyMode_Walk final : public CPlayerState_Base 
{
private:
		 CPlayerState_BloodyMode_Walk();
virtual ~CPlayerState_BloodyMode_Walk() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer) override;
	virtual HRESULT StartState() override;
	virtual HRESULT EndState() override;

	virtual void	Tick(const _float& fTimeDelta) override;
	virtual void	Late_Tick(const _float& fTimeDelta) override;

private:
	virtual void	KeyInput(const _float& fTimeDelta) override;

private:
	_bool			m_bKeyPressing = false;


public:
	static CPlayerState_BloodyMode_Walk* Create(CPlayer* pPlayer);
	virtual void Free() override;
};

END

