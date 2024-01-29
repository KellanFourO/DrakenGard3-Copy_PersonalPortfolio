#pragma once

#include "Base.h"

BEGIN(Engine)

class CCollider;

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc
	{
		tagGameObjectDesc() {}
		tagGameObjectDesc(_float fSpeed, _float fRotation)
			: fSpeedPerSec(fSpeed), fRotationPerSec(fRotation) {}

		_float fSpeedPerSec		= 0.f;
		_float fRotationPerSec	= 0.f;
		_uint  iLevelIndex = 0;
		_uint  iCellIndex = -1;
	
	}GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	class CTransform*		Get_Transform() { return m_pTransformCom; }
	vector<class CCollider*>& Get_Colliders() { return m_vecColliders; }
	_float					Get_LifeTime() { return m_fLifeTime; }
	_float3&				Get_Offset() { return m_vCameraOffset; }
	_float3&				Get_JumpOffset() { return m_vJumpOffset; }

	_bool					Is_Player() { return m_isPlayer; }
	_bool					Is_Dead()	{ return m_isDead; }
	void					Set_Move(_bool bMove) { m_bMove = bMove; }
	void					Set_Dead() { m_isDead = true; }
	void					Die(_float fLifeTime) { m_fLifeTime = fLifeTime; m_isDead = true; }
	void					Set_CellIndex(_int iCellIndex ) { m_iCellIndex = iCellIndex; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Priority_Tick(_float fTimeDelta);
	virtual void	Tick(_float fTimeDelta);
	virtual void	Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	_bool	Picking(_float3 vPickPos, class CModel* pModelCom, _float3* pOut);
	void	SetStartAppeal(_bool bAppeal) { m_bStartAppeal = bAppeal;}

public:
	virtual void	  Write_Json(json& Out_Json) override;
	virtual void	  Load_FromJson(const json& In_Json) override;
	class CComponent* Find_Component(const wstring& strComTag);
	class CPartObject* Find_PartObject(const wstring& strPartTag);
	
	void			   Find_Collider();
	
public:
	virtual void On_Collision(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit); // call on collising
	virtual void On_CollisionEnter(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit);
	virtual void On_CollisionExit(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit);

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

protected:
	class CGameInstance*		m_pGameInstance = { nullptr };

protected:
	class CTransform*			m_pTransformCom = { nullptr };

	map<const wstring, class CComponent*>		m_Components;
	map<const wstring, class CPartObject*>		m_PartObjects;
	vector<class CCollider*>					m_vecColliders;
	

protected:
	_bool						m_isCloned = { false };
	_bool						m_isPlayer = { false };
	_bool						m_isDead = { false };
	_bool						m_bMove = true;
	_bool						m_bStartAppeal = false;
	_int						m_iCellIndex = -1;
	
	_float						m_fLifeTime = 5.f;
	_float3						m_vCameraOffset = {};
	_float3						m_vJumpOffset = {};
	_bool						m_bChangeOffset = false;
	

protected:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag,
			const wstring& strComTag, _Inout_ CComponent** ppOut, void* pArg = nullptr);

	void	Delete_Component(const wstring& strComTag);

	

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END