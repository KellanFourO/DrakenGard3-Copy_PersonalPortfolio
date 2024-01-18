#pragma once

#include "Component.h"

#include "BoundingBox_AABB.h"
#include "BoundingBox_OBB.h"
#include "BoundingBox_Sphere.h"

//!/* 충돌체. */
//!/* 충돌체를 정의하는 기능 + 충돌체을 그리는 기능 + 충돌을 비교하는 기능 . */

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_Sphere, TYPE_AABB, TYPE_OBB, TYPE_END };
	enum PARTTYPE { PART_BODY, PART_WEAPON, PARTTYPE_END };

public:
	enum COLLISION_STATE
	{
		COLLISION_STATE_NONE,
		COLLISION_STATE_ENTER,
		COLLISION_STATE_ON,
		COLLISION_STATE_EXIT,
		COLLISION_STATE_END,
	};

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	TYPE Get_ColliderType() const {	return m_eType;	}
	COLLISION_STATE Get_State() const { return m_eState; }
	_uint Get_ID() { return m_iID; }

	class CBoundParent* Get_Bounding() { return m_pBounding; }

	PARTTYPE Get_PartType() const { return m_ePartType; }
	void	 Set_PartType(PARTTYPE ePartType) { m_ePartType = ePartType; }

public:
	virtual HRESULT Initialize_Prototype(TYPE eType);
	virtual HRESULT Initialize(void* pArg);

public:
	void	Update(_fmatrix TransformMatrix);
	_bool	Collision(CCollider* pTargetCollider);

	_bool   isOnCollider() { return m_bOnCollider; }
	void	OnCollider() { m_bOnCollider = true; }
	void    OffCollider() { m_bOnCollider = false; }

	_bool	isAccCollider() { return m_bAccCollider; }
	void	OnAccCollider(_float fHitTick) { m_bAccCollider = true; m_fHitTickTime = fHitTick; }
	void	OffAccCollider() { m_bAccCollider = false; m_fHitTickTime = 0.f;}

	_float	Get_HitTick() { return m_fHitTickTime; }
	


public:
	void On_Collision(CCollider* pOther, const _float& fX, const _float& fY, const _float& fZ); // call on collising
	void On_CollisionEnter(CCollider* pOther, const _float& fX, const _float& fY, const _float& fZ);
	void On_CollisionExit(CCollider* pOther, const _float& fX, const _float& fY, const _float& fZ);
	
public:
	static	_uint			g_iColliderID;
	_uint					m_iID = { 0 };

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	TYPE					m_eType = { TYPE_END };
	PARTTYPE				m_ePartType = { PARTTYPE_END };
	class CBoundParent*		m_pBounding = { nullptr };
	_bool					m_isCollision = { false };

	_bool					m_bOnCollider = false;

	COLLISION_STATE			m_eState = { COLLISION_STATE_END };
	

	_bool					m_bAccCollider = false; //! 만약, 데미지가 누적해서 들어가야 한다면.
	_float					m_fHitTickTime = 0.f; //! 일정시간 마다 데미지가 한번씩 들어가게 하자.
	
	

private:
	PrimitiveBatch<VertexPositionColor>*	m_pBatch = { nullptr };
	BasicEffect*							m_pEffect = { nullptr };
	ID3D11InputLayout*						m_pInputLayout = { nullptr };


public:
	static CCollider*		Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END