#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CModel;

class ENGINE_DLL CStateBase abstract : public CBase
{
protected:
		 CStateBase();
virtual ~CStateBase() = default;

public:
	virtual HRESULT Initialize(class CModel* pOwnerModel); //! ���� ������ ���� �ؾ��ϴ� ���� ��������
	virtual HRESULT StartState(); //! �ٽ� �ڱ� �ڽ��� ȣ�� �� ���� �ൿ�� ��������
	virtual HRESULT EndState();   //! �ڱ� �ڽ��� ȣ���� ���� �� ���� �ൿ�� ��������. ex) Reset

	virtual void Priority_Tick(const _float& fTimeDelta);
	virtual void Tick(const _float& fTimeDelta);
	virtual void Late_Tick(const _float& fTimeDelta);
	
protected:
	class CModel*		 m_pOwnerModelCom = nullptr; //! ���¿� ���� �ִϸ��̼� ��ü�� ���� �������� �� ������Ʈ
	class CGameInstance* m_pGameInstance = nullptr;
public:
	virtual void Free() override;
};

END

