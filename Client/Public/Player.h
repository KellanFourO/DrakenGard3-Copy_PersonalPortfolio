#pragma once

#include "Client_Defines.h"
#include "AnimObject.h"

BEGIN(Engine)
class CCollider;
class CNavigation;
class CRigidBody;
class CStateMachine;
class CStateBase;
class CPartObject;
END

BEGIN(Client)

class CCamera_Target;
class CPlayerPart_Body;
class CEffect_Trail;



class CPlayer final : public CAnimObject
{

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;



public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void On_Collision(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit) override; // call on collising
	virtual void On_CollisionEnter(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit) override;
	virtual void On_CollisionExit(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit) override;

public:
	
	CCamera_Target*			Get_Cam() { return m_pCamera; }
	void					Set_Cam(class CCamera_Target* pCam);
	CPartObject*			Find_PartObject(const wstring& strPartTag);
	LEVEL					Get_LevelID() { return m_eCurrentLevelID; }
		_float				Get_Dmg() { return m_tStatus.fDmg; }
	STATUS_DESC::ATTACKTYPE Get_AttackType() { return m_tStatus.eAttackType; }
	void					Init_Status(_float fMaxHp, _float fDmg);
	void					Transition(CStateMachine::STATETYPE eStateType, wstring& strStateTag);
	
public:
	void					On_SwordTrail();
	void					Off_SwordTrail();
	CEffect_Trail*			Get_Trail();

private:
	HRESULT			Ready_Components();
	HRESULT			Ready_PartObjects();
	HRESULT			Ready_States();
	HRESULT			Ready_Camera();
	HRESULT			Add_PartObject(const wstring& strPrototypeTag, const wstring& strPartTag, void* pArg);
	void			Key_Input(const _float fTimeDelta);

	

	string			ConvertWstrToStrTest(const wstring& wstr);
private:
	CNavigation*		m_pNavigationCom = { nullptr };
	CCollider*			m_pColliderCom	 = { nullptr };
	
	CRigidBody*			m_pRigidBodyCom  = { nullptr };
	CStateMachine*		m_pStateCom		 = { nullptr };

	CCamera_Target*		m_pCamera		= { nullptr };
private:
	_bool				m_bAdmin = false;
	_bool				m_bSturn = false;
	
	_float				m_fAccTime = 0.f;

	STATUS_DESC							m_tStatus = {};
	STATUS_DESC							m_tOriginStatus = {};
	_bool				m_bTest = true;

	
	

private:
	//map<const wstring, class CPartObject*>			m_PartObjects;

public:
	/* 원형객체를 생성한다. */
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END