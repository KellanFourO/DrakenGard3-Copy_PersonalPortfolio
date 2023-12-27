#pragma once
#include "PlayerState_Base.h"

BEGIN(Engine)
class CModel;
class CStateMachine;
END

BEGIN(Client)
class CPlayer;

class CPlayerState_NormalAttackBase abstract : public CPlayerState_Base
{
protected:
		CPlayerState_NormalAttackBase();
virtual ~CPlayerState_NormalAttackBase() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer);
	virtual HRESULT StartState() override;
	virtual HRESULT EndState() override;

	virtual void	Tick(const _float& fTimeDelta) override;

protected:
	virtual void	NextComboOrIdle(class CModel* pOwnerModel, class CStateMachine* pOwnerStateMachine, const wstring& strNextComboStateTag, _int iEndAnimIndex);

protected:
	_bool	m_isEnd = false;
	_bool	m_isChange = false;

public:
	virtual void Free() override;
};

END
