#pragma once
#include "AnimObject.h"
#include "BrainTree/BrainTree.h"

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

public:
	virtual void On_Collision(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType) override; // call on collising
	virtual void On_CollisionEnter(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType) override;
	virtual void On_CollisionExit(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType) override;

protected:
	virtual HRESULT						Ready_BehaviorTree();

protected:
	CShader*							m_pShaderCom	= { nullptr };
	CModel*								m_pModelCom		= { nullptr }; //#버퍼컴에서_모델컴으로_변경됨
	CCollider*							m_pColliderCom	= { nullptr };
	CRigidBody*							m_pRigidBodyCom = { nullptr };

	CNavigation*						m_pNavigationCom = { nullptr };
	shared_ptr<BrainTree::BehaviorTree> m_BehaviorTree	= { nullptr };

private:
	//STATE_LINK_MONSTER_DESC      m_tLinkStateDesc;

public:
	virtual void Free() override;

};

END

