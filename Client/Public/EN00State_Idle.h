#pragma once
#include "AIState_Base.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CEN00State_Idle final : public CAIState_Base
{
private:
		CEN00State_Idle();
virtual ~CEN00State_Idle() = default;


public:
	virtual HRESULT Initialize(CGameObject* pOwner); //! ���� ������ ���� �ؾ��ϴ� ���� ��������
	virtual HRESULT StartState() override; //! �ٽ� �ڱ� �ڽ��� ȣ�� �� ���� �ൿ�� ��������
	virtual HRESULT EndState() override;   //! �ڱ� �ڽ��� ȣ���� ���� �� ���� �ൿ�� ��������. ex) Reset

	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;

public:
	static CEN00State_Idle*	Create(CGameObject* pOwner);
	virtual void			Free();
};

END


