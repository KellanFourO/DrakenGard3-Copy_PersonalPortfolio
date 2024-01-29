#pragma once

#include "Client_Defines.h"
#include "NonAnimObject.h"

BEGIN(Engine)
class CCollider;
END


BEGIN(Client)

class CBullet abstract : public CNonAnimObject
{
public:
	typedef struct tagBulletDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float	  fDmg; //! 공격력
		_float	  fRange; // ! 갈 수 있는 범위
		_float	  fDeadTime; //! 죽어야할 시간
		_float4   vLook; //! 가야 할 방향을 던져주자
		_float4x4 OwnerWorldMatrix;
		_float4	  vTargetPos;
		
	}BULLET_DESC;

protected:
	CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBullet(const CBullet& rhs);
	virtual ~CBullet() = default;

public:
	_float	Get_Bullet_Dmg() { return m_fDmg; }
	_float	Get_Bullet_Range() { return m_fRange; }
	_float	Get_Dmg() { return m_tStatus.fDmg; }
	STATUS_DESC::ATTACKTYPE Get_AttackType() { return m_tStatus.eAttackType; }

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					Tick(_float TimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT					Render();

public:
	virtual void On_Collision(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit); // call on collising
	virtual void On_CollisionEnter(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit);
	virtual void On_CollisionExit(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit);

public:
	void							Go_Straight(_float fTimeDelta);
	void							Go_Parabolic(_float fTimeDelta);
	void							Dead_Range(_float fRange);
	virtual void					Initialize_Pos(_fvector vIntializePos);
	virtual void					Init_Status(_float fMaxHp, _float fDmg);

	


protected:
	CCollider*						m_pColliderCom = { nullptr };
	_float4x4						m_OwnerWorldMatrix;
	_float4							m_vPrevPos = {};

	_bool							m_bParabolic = false;
	_bool							m_bGravity = false;

protected:
	STATUS_DESC							m_tStatus = {};
	STATUS_DESC							m_tOriginStatus = {};

	_float		m_fDmg = { 0.f };
	_float		m_fRange = { 12.f };
	_float		m_fTimeAcc = 0.f;
	_float		m_fLookTime = 0.7f;
	_bool		m_bOneTick = true;
	_float		m_fGravitionalConstant = 9.8f;
	_float3		m_vVelocity = {};

	_bool		m_bHitDead = true; // ! 총알이 타겟과 충돌시 사라질것인가?
	
	//! 포물선
	
	_float4		m_vTargetPos = {};
	
	
	

public:
	virtual void					Free() override;
};

END	

