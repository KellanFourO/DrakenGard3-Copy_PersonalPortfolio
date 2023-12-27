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
	virtual HRESULT Initialize(class CModel* pOwnerModel); //! 최초 생성시 셋팅 해야하는 값을 정의하자
	virtual HRESULT StartState(); //! 다시 자기 자신이 호출 될 때의 행동을 정의하자
	virtual HRESULT EndState();   //! 자기 자신이 호출이 종료 될 때의 행동을 정의하자. ex) Reset

	virtual void Priority_Tick(const _float& fTimeDelta);
	virtual void Tick(const _float& fTimeDelta);
	virtual void Late_Tick(const _float& fTimeDelta);
	
protected:
	class CModel*		 m_pOwnerModelCom = nullptr; //! 상태에 따른 애니메이션 교체를 위한 소유자의 모델 컴포넌트
	class CGameInstance* m_pGameInstance = nullptr;
public:
	virtual void Free() override;
};

END

