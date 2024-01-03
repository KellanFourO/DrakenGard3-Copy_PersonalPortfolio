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
	virtual HRESULT Initialize(CPlayer* pPlayer); //! ���� ������ ���� �ؾ��ϴ� ���� ��������
	virtual HRESULT StartState() = 0; //! �ٽ� �ڱ� �ڽ��� ȣ�� �� ���� �ൿ�� ��������
	virtual HRESULT EndState() = 0;

	virtual void	Tick(const _float& fTimeDelta) = 0;

protected:
	virtual void	KeyInput(const _float& fTimeDelta);
	void			MouseInput(const _float& fTimeDelta);
	void			RootMotion();

protected:
	void			Vertical_Camera_Rotate(); //! ī�޶� �� ���� �������� ���� ȸ��
	void			Vertical_Camera_RotateTest(); //! ī�޶� �� ���� �������� ���� ȸ��

	void			Horizon_Camera_Rotate(); //! ī�޶� �� ���� ���� ���� 90�� ȸ��
	

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
	
	_bool			m_bInput = false; //! End�� �ش��ϴ� ������Ʈ���� Ű�Է�(���Է�)�� �޾Ҵٸ� ���� �޺��� �����Ű������.
	
	
	_bool			m_bVerticalTurn = false;
	_bool			m_bHorizontalTurn = false;



	_float			m_fTurnAngle = 0.f;

public:
	virtual void Free() override;


};

END