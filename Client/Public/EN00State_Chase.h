#pragma once
#include "AIState_Base.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CEN00State_Chase final : public CAIState_Base
{
private:
		CEN00State_Chase();
virtual ~CEN00State_Chase() = default;


public:
	virtual HRESULT Initialize(CGameObject* pOwner); //! 최초 생성시 셋팅 해야하는 값을 정의하자
	virtual HRESULT StartState() override; //! 다시 자기 자신이 호출 될 때의 행동을 정의하자
	virtual HRESULT EndState() override;   //! 자기 자신이 호출이 종료 될 때의 행동을 정의하자. ex) Reset

	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;

public:
	static CEN00State_Chase*	Create(CGameObject* pOwner);
	virtual void			Free();
};

END


