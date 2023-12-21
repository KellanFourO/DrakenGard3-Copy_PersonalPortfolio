#pragma once
#include "PlayerState_Base.h"

BEGIN(Client)

class CPlayerState_Walk final : public CPlayerState_Base 
{
private:
		 CPlayerState_Walk();
virtual ~CPlayerState_Walk() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer);
	virtual HRESULT Priority_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Tick(const _float& fTimeDelta) override;
	virtual HRESULT Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Transition() override;

private:
	virtual void   KeyInput(const _float& fTimeDelta) override;
	virtual void   ResetState() override;

private:
	_bool		   m_bKeyPressing = false;

public:
	static CPlayerState_Walk* Create(CPlayer* pPlayer);
	virtual void Free() override;
};

END

