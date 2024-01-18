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
	
	}GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	class CTransform*		Get_Transform() { return m_pTransformCom; }
	_bool					Is_Player() { return m_isPlayer; }
	_bool					Is_Dead()	{ return m_isDead; }
	void					Die() { m_isDead = true; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Priority_Tick(_float fTimeDelta);
	virtual void	Tick(_float fTimeDelta);
	virtual void	Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	_bool	Picking(_float3 vPickPos, class CModel* pModelCom, _float3* pOut);


public:
	virtual void	  Write_Json(json& Out_Json) override;
	virtual void	  Load_FromJson(const json& In_Json) override;
	class CComponent* Find_Component(const wstring& strComTag);
	class CPartObject* Find_PartObject(const wstring& strPartTag);
	
	class CCollider*	Find_Collider(_bool bPartType);
	
public:
	virtual void On_Collision(CGameObject* pLeftObject, wstring& LeftTag, CGameObject* pRightObject, wstring& RightTag); // call on collising
	virtual void On_CollisionEnter(CGameObject* pLeftObject, wstring& LeftTag, CGameObject* pRightObject, wstring& RightTag);
	virtual void On_CollisionExit(CGameObject* pLeftObject, wstring& LeftTag, CGameObject* pRightObject, wstring& RightTag);

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

protected:
	class CGameInstance*		m_pGameInstance = { nullptr };

protected:
	class CTransform*			m_pTransformCom = { nullptr };

	map<const wstring, class CComponent*>		m_Components;
	map<const wstring, class CPartObject*>		m_PartObjects;

protected:
	_bool						m_isCloned = { false };
	_bool						m_isPlayer = { false };
	_bool						m_isDead = { false };
	

protected:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag,
			const wstring& strComTag, _Inout_ CComponent** ppOut, void* pArg = nullptr);

	void	Delete_Component(const wstring& strComTag);

	

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END