#pragma once
#include "StateBase.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CStateMachine;
class CTransform;
class CNavigation;
class CRigidBody;
END

BEGIN(Client)
class CPlayer;



class CPlayerState_Base abstract : public CStateBase
{
public:
	enum PlayerDir { LEFT, RIGHT, FRONT, BACK, DIR_END };

protected:
	CPlayerState_Base();
	virtual ~CPlayerState_Base() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer); //! 최초 생성시 셋팅 해야하는 값을 정의하자
	virtual HRESULT StartState() = 0; //! 다시 자기 자신이 호출 될 때의 행동을 정의하자
	virtual HRESULT EndState() = 0;

	virtual void	Tick(const _float& fTimeDelta) = 0;

protected:
	virtual void	KeyInput(const _float& fTimeDelta);
	void			MouseInput(const _float& fTimeDelta);
	void			RootMotion();

protected:
	void			Vertical_Camera_Rotate(); //! 카메라 룩 벡터 기준으로 수직 회전
	void			Vertical_Camera_RotateTest(); //! 카메라 룩 벡터 기준으로 수직 회전

	void			Horizon_Camera_Rotate(); //! 카메라 룩 벡터 기준 수평 90도 회전
	

protected:
	CStateMachine*	m_pOwnerStateCom = { nullptr };
	CTransform*		m_pOwnerTransform = { nullptr };
	CNavigation*	m_pOwnerNavagation = { nullptr };
	CRigidBody*		m_pOwnerRigidBody = { nullptr };

	class CCamera_Target* m_pOwnerCam = { nullptr };
	
	
	
protected:
	_float			m_fLastInputTime = { 0.0f };
	_float			m_fEndTime = 0.0f;
	_float			m_fAccTime = 0.0f;

	_float			m_fMouseX = 0.f, m_fMouseY = 0.f;
	_float			m_fMouseSensitivity = 0.05f;

	PlayerDir		m_ePrevDir = DIR_END;
	PlayerDir		m_eCurrentDir = FRONT;

	_bool			m_bFront = true;
	
	_bool			m_bInput = false; //! End에 해당하는 스테이트에서 키입력(선입력)을 받았다면 다음 콤보로 연계시키기위함.
	
	
	_bool			m_bVerticalTurn = false;
	_bool			m_bHorizontalTurn = false;



	_float			m_fTurnAngle = 0.f;

public:
	virtual void Free() override;


};

END