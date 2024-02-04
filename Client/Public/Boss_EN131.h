#pragma once
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CModel;
END

BEGIN(Client)


class CBoss_EN131 final : public CMonster
{
private:
	enum HEAD_COLLIDER { LEFT, CENTER, RIGHT, HEAD_COLLIDER_END };
	enum TAIL_COLLIDER { STING, SWING, TAIL_COLLIDER_END };

private:
	CBoss_EN131(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_EN131(const CBoss_EN131& rhs);
	virtual ~CBoss_EN131() = default;

public:
	CPartObject* Find_PartObject(const wstring& strPartTag);
	_bool		Get_Appear() { return m_bAppear; }
	void		Start_Appear() { m_bStart = true;}
	

private:
	CCollider* Get_HeadCollider(HEAD_COLLIDER eHeadType);
	CCollider* Get_TailCollider(TAIL_COLLIDER eTailType);
	void	   Create_TailStingEffect();
	

public:
	virtual HRESULT Initialize_Prototype(LEVEL eLevel) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public:
	virtual void Write_Json(json& Out_Json) override;
	virtual void Load_FromJson(const json& In_Json) override;
	virtual void Init_Desc() override;
	
public:
	virtual void On_Collision(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit) override; // call on collising
	virtual void On_CollisionEnter(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit) override;
	virtual void On_CollisionExit(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit) override;

private:
	HRESULT			Ready_Components();
	HRESULT			Ready_PartObjects();
	virtual HRESULT	Ready_BehaviorTree_V2() override;
	HRESULT			Add_PartObject(const wstring& strPrototypeTag, const wstring& strPartTag, void* pArg);
	
	HRESULT			Bind_ShaderResources();

	

private:
	_bool			m_bAppear = false;
	_bool			m_bStart = false;
	_bool			m_bJumpStart = false;
	_bool			m_bTest = false;
	_bool			m_bFindCell = false;
	_bool			m_bChase = false;
	_float			m_fTimeAcc = 0.f;
	_float			m_fPointJumpTimeAcc = 0.f;

	
	_float3			m_vVelocity = {};
	_float			m_fPointJumpGravity = {0.f};
	_float			m_fMaxHeight = 25.f;
	_float			m_fMaxTime = 1.f;
	_float3			m_vPrevPointPos = {};
	_float3			m_vAppealJumpPosition = { 10.024f, 19.229f, 240.570f };
	_bool			m_bOneTime = true;


	_float			m_fPearCreateTime = 0.1f;
	

public:
	/* 원형객체를 생성한다. */
	static CBoss_EN131* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;

};

END

