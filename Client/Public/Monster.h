#pragma once
#include "AnimObject.h"
#include "BrainTree/BrainTree.h"

using namespace BrainTree;

BEGIN(Engine)
class CShader;
class CCollider;
class CModel;
class CNavigation;
class CRigidBody;
END


BEGIN(Client)

class CMonster abstract : public CAnimObject
{
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT		Initialize_Prototype(LEVEL eLevel);
	virtual HRESULT		Initialize(void* pArg);
	virtual void		Priority_Tick(_float fTimeDelta);
	virtual void		Tick(_float fTimeDelta);
	virtual void		Late_Tick(_float fTimeDelta);
	virtual HRESULT		Render();

public:
	virtual void		Write_Json(json& Out_Json);
	virtual void		Load_FromJson(const json& In_Json);
	virtual void		Init_Desc();
	virtual void		Init_Status(_float fMaxHp, _float fDmg);

	_float				Get_CurrentHpRatio() { return m_pBehaviorTree->getBlackboard()->getFloat("Current_HP") / m_pBehaviorTree->getBlackboard()->getFloat("Max_HP"); } ;
	_float3				Get_TargetPosition() { return m_pBehaviorTree->getBlackboard()->getFloat3("TargetPosition"); }
	_float3				Get_MyPosition() { return m_pBehaviorTree->getBlackboard()->getFloat3("MyPosition"); }
	_float3				Get_DashPosition() { return m_pBehaviorTree->getBlackboard()->getFloat3("DashRushTargetPosition"); }
	_float3				Get_EscapePosition() { return m_pBehaviorTree->getBlackboard()->getFloat3("EscapePosition"); }
	_float4				Create_RandomPosition(_float3 vCurrentPosition, _float fMinDistance, _float fMaxDistance);
	_bool				Is_CurrentAnimEnd();
	void				Set_AnimSpeed(_float fSpeed);
	void				Reset_AnimSpeed() { m_pModelCom->Set_AnimationSpeed(1.5f);}

	_float				Get_Dmg() { return m_tStatus.fDmg; }
	STATUS_DESC::ATTACKTYPE Get_AttackType() { return m_tStatus.eAttackType;}
	CCollider* Get_WeaponCollider();

public:
	virtual void On_Collision(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit) override; // call on collising
	virtual void On_CollisionEnter(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit) override;
	virtual void On_CollisionExit(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit) override;

public:
	virtual void Transition(_int iAnimIndex, _float fSpeed = 1.5f);

protected:
	virtual HRESULT						Ready_BehaviorTree();
	virtual HRESULT						Ready_BehaviorTree_V2();

protected:
	CShader*							m_pShaderCom	= { nullptr };
	CModel*								m_pModelCom		= { nullptr }; //#버퍼컴에서_모델컴으로_변경됨
	CCollider*							m_pColliderCom	= { nullptr };
	CRigidBody*							m_pRigidBodyCom = { nullptr };

	CNavigation*						m_pNavigationCom = { nullptr };
	shared_ptr<Node>					m_pBehaviorTree = { nullptr };

	STATUS_DESC							m_tStatus = {};
	STATUS_DESC							m_tOriginStatus = {};
private:
	//STATE_LINK_MONSTER_DESC      m_tLinkStateDesc;

public:
	virtual void Free() override;

};

END

