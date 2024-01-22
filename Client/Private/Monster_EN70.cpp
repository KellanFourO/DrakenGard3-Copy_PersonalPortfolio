#include "stdafx.h"
#include "Monster_EN70.h"
#include "GameInstance.h"

#include "PartObject.h"
#include "Bone.h"
#include "Animation.h"



CMonster_EN70::CMonster_EN70(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice,pContext)
{
}

CMonster_EN70::CMonster_EN70(const CMonster_EN70& rhs)
	: CMonster(rhs)
{
}

CPartObject* CMonster_EN70::Find_PartObject(const wstring& strPartTag)
{
	auto iter = m_PartObjects.find(strPartTag);

	if (iter == m_PartObjects.end())
		return nullptr;

	return iter->second;
}

HRESULT CMonster_EN70::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelID = eLevel;
	
	return S_OK;
}

HRESULT CMonster_EN70::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	Find_Collider();

	if (m_eCurrentLevelID != LEVEL_TOOL)
	{

		if(FAILED(Ready_BehaviorTree_V2()))
			return E_FAIL;
	}

	m_pModelCom->Root_MotionStart();

	return S_OK;
}

void CMonster_EN70::Priority_Tick(_float fTimeDelta)
{

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Priority_Tick(fTimeDelta);
	}
}

void CMonster_EN70::Tick(_float fTimeDelta)
{
	if (m_eCurrentLevelID != LEVEL_TOOL)
	{
		Blackboard::Ptr pBlackBoard = m_pBehaviorTree->getBlackboard();
		
		_float3 vMyPos, vTargetPos;
		XMStoreFloat3(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		XMStoreFloat3(&vTargetPos, pBlackBoard->GetTarget()->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		
		pBlackBoard->setFloat3("MyPosition", vMyPos);
		pBlackBoard->setFloat3("TargetPosition", vTargetPos);

		

		Node::Status TreeStatus = m_pBehaviorTree->update(fTimeDelta);
	}

	

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Tick(fTimeDelta);
	}


	_float3 vPos = {};

	m_pModelCom->Play_Animation(fTimeDelta, vPos);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	
	if (m_eCurrentLevelID != LEVEL_TOOL)
		m_pRigidBodyCom->Tick(fTimeDelta);

	_vector vRealPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vRealPos.m128_f32[0] += vPos.x;
	vRealPos.m128_f32[1] += vPos.y;
	vRealPos.m128_f32[2] += vPos.z;
	vRealPos.m128_f32[3] = 1.f;

	if (m_eCurrentLevelID != LEVEL_TOOL)
	{
		if (true == m_pNavigationCom->isMove(vRealPos) && true == m_bMove)
			m_pTransformCom->Add_LookPos(vPos);
	}
}

void CMonster_EN70::Late_Tick(_float fTimeDelta)
{

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Late_Tick(fTimeDelta);
	}

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CMonster_EN70::Render()
{
	//#몬스터모델렌더

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//TODO 클라에서 모델의 메시 개수를 받아와서 순회하면서 셰이더 바인딩해주자.

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		m_pShaderCom->Begin(0); //! 셰이더에 던져주고 비긴 호출하는 걸 잊지말자

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG

	m_pColliderCom->Render();

#endif

	return S_OK;
}

void CMonster_EN70::Write_Json(json& Out_Json)
{
	__super::Write_Json(Out_Json);
	
}

void CMonster_EN70::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);

}

void CMonster_EN70::Init_Desc()
{
	//m_pStatus.lock()->Init_Status(&m_tLinkStateDesc);
}

void CMonster_EN70::On_Collision(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit)
{
	__super::On_Collision(pCollisionObject, LeftTag, RightTag, vCollisionPos, bType, bHit);

	if (RightTag == TEXT("Layer_Player"))
	{
		_float magnitude = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vCollisionPos)));


		_vector vLook = pCollisionObject->Get_Transform()->Get_State(CTransform::STATE_LOOK);
		_vector vForceDir = vLook * magnitude;

		_float3 vCalcPos;
		XMStoreFloat3(&vCalcPos, vForceDir);

		m_pRigidBodyCom->Add_Force(vCalcPos, CRigidBody::FORCE_MODE::FORCE);
	}

	if (RightTag == TEXT("Layer_Monster"))
	{
		_float magnitude = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vCollisionPos)));

		_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		_vector vForceDir = vRight * magnitude;

		_float3 vCalcPos;
		XMStoreFloat3(&vCalcPos, vForceDir);
		m_pRigidBodyCom->Add_Force(vCalcPos, CRigidBody::FORCE_MODE::FORCE);

		vCalcPos = { -vCalcPos.x, -vCalcPos.y, -vCalcPos.z };

		CRigidBody* pTargetBody = dynamic_cast<CRigidBody*>(pCollisionObject->Find_Component(TEXT("Com_RigidBody")));
		
		pTargetBody->Add_Force(vCalcPos, CRigidBody::FORCE_MODE::FORCE);

	}
}

void CMonster_EN70::On_CollisionEnter(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit)
{
	
	__super::On_CollisionEnter(pCollisionObject, LeftTag, RightTag, bType, bHit);
}

void CMonster_EN70::On_CollisionExit(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit)
{
	__super::On_CollisionExit(pCollisionObject, LeftTag, RightTag, bType, bHit);

	m_pRigidBodyCom->Clear_NetPower();
}

HRESULT CMonster_EN70::Ready_Components()
{
	CNavigation::NAVI_DESC		NaviDesc = {};
	NaviDesc.iCurrentIndex = m_iCellIndex;

	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	/* For.Com_Shader */ //#모델셰이더는_Prototype_Component_Shader_Model
	//Prototype_Component_Shader_AnimModel
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_AnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Model_Monster_EN70"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBoundingBox_AABB::BOUNDING_AABB_DESC		BoundingDesc = {};

	BoundingDesc.vExtents = _float3(0.4f, 0.7f, 0.4f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	BoundingDesc.ePartType = CBoundParent::PARTTYPE_BOUND::PART_BODY;

	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;


	m_pColliderCom->Set_PartType(CCollider::PART_BODY);
	m_pColliderCom->OnCollider();

	CRigidBody::RIGIDBODY_TYPE eType = CRigidBody::RIGIDBODY_TYPE::DYNAMIC;

	////TODO 리지드바디
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &eType)))
		return E_FAIL;

	m_pRigidBodyCom->Set_Owner(this);

	m_pRigidBodyCom->Set_OwnerNavigation(m_pNavigationCom);
	m_pRigidBodyCom->Clear_NetPower();
	m_pRigidBodyCom->Set_UseGravity(true);
	
	return S_OK;
}

HRESULT CMonster_EN70::Ready_PartObjects()
{
	CPartObject::PART_DESC WeaponDesc = {};

	
	
	CBone* pSwordBone = m_pModelCom->Get_BonePtr("M_WP1");

	if(nullptr == pSwordBone)
		return E_FAIL;

	WeaponDesc.m_pSocketBone = pSwordBone;
	WeaponDesc.m_pParentTransform = m_pTransformCom;


	if(FAILED(Add_PartObject(TEXT("Prototype_PartObject_Monster_EN70_Weapon"), TEXT("Part_Weapon"), &WeaponDesc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CMonster_EN70::Ready_BehaviorTree_V2()
{
	m_pBehaviorTree = std::make_shared<BrainTree::BehaviorTree>();

	 BrainTree::Blackboard::Ptr EN00_BlackBoard = m_pBehaviorTree->getBlackboard();
	 	
	 EN00_BlackBoard->setString("Name", "EN70");
	 EN00_BlackBoard->setFloat("Max_HP", 200.f);
	 EN00_BlackBoard->setFloat("Current_HP", 200.f);
	 EN00_BlackBoard->setFloat("Attack_Range", 3.f);
	 EN00_BlackBoard->setFloat("Keep_Range", 3.5f);
	 EN00_BlackBoard->setFloat("Detect_Range", 10.f);
	 EN00_BlackBoard->setFloat("Speed", m_pTransformCom->Get_Speed());
	 EN00_BlackBoard->setFloat("RotateSpeed", m_pTransformCom->Get_RotationSpeed());
	 

	 CGameObject* pTarget = nullptr;

	 pTarget = m_pGameInstance->Get_Player(m_eCurrentLevelID);

	 if (nullptr != pTarget)
	 {
		 EN00_BlackBoard->setTarget(pTarget);
	 }

	 _float3 vMyPos, vTargetPos;
	 
	 XMStoreFloat3(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	 XMStoreFloat3(&vTargetPos, pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION));

	 EN00_BlackBoard->setFloat3("MyPosition", vMyPos);
	 EN00_BlackBoard->setFloat3("TargetPosition", vTargetPos);

	 //EN00_BlackBoard->setFloat3("TargetPostion", _float3(0.f, 0.f, 0.f));
	 
	 EN00_BlackBoard->setBool("Is_Hit", false); //! 피격중인가
	 EN00_BlackBoard->setBool("Is_Ground", true);
	 EN00_BlackBoard->setBool("Is_Dead", false);
	 //EN00_BlackBoard->setBool("Is_TargetPosition", false); //! 목표로 하는 지점이 있는가
	 EN00_BlackBoard->setBool("Is_Patrol", false); //! 순찰해야하는가?
	 EN00_BlackBoard->setBool("Is_OneTick", true);
	 EN00_BlackBoard->setInt("iRandomIndex", -1);
	 EN00_BlackBoard->setOwner(this);
	 EN00_BlackBoard->setModel(m_pModelCom);
	 EN00_BlackBoard->setShader(m_pShaderCom);
	 EN00_BlackBoard->setCollider(m_pColliderCom);
	 EN00_BlackBoard->setTransform(m_pTransformCom);
	 EN00_BlackBoard->setGameInstance(m_pGameInstance);
	 EN00_BlackBoard->setNavigation(m_pNavigationCom);
	 EN00_BlackBoard->setRigidBody(m_pRigidBodyCom);
	 EN00_BlackBoard->setInt("iDirState", 0); //! 0은 정면, 1은 좌측, 2는 우측

	 
	 
	 FUNCTION_NODE Control_IsDead
	 = FUNCTION_NODE_MAKE
	 {
		 if (true == pBlackboard->getBool("Is_Dead"))
		 {
			_vector vTargetPos = XMLoadFloat3(&pBlackboard->getFloat3("TargetPosition"));
			vTargetPos.m128_f32[3] = 1;


			m_pModelCom->Set_Loop(false);

			if (true == pBlackboard->getBool("Is_OneTick"))
			{
				_int iRandomHitIndex = Random({ 30, 31 });
				pBlackboard->setInt("iRandomIndex", iRandomHitIndex);

				m_pTransformCom->Look_At(vTargetPos);
				Transition(iRandomHitIndex, 1.5f);
				pBlackboard->setBool("Is_OneTick", false);

			}
			
			return BT_STATUS::Success;
		  }
		  else
			return BT_STATUS::Failure;

	 };

	FUNCTION_NODE Task_IsDead
		 = FUNCTION_NODE_MAKE
	 {
			Die(5.f);
			return BT_STATUS::Success;
	 };

	 FUNCTION_NODE Control_IsHit
	 = FUNCTION_NODE_MAKE
	 {
		 if (true == pBlackboard->getBool("Is_Hit"))
		 {	
			_vector vTargetPos = XMLoadFloat3(&pBlackboard->getFloat3("TargetPosition"));
			vTargetPos.m128_f32[3] = 1;
			

			m_pModelCom->Set_Loop(false);

			if (true == pBlackboard->getBool("Is_OneTick"))
			{
				_int iRandomHitIndex = Random({ 15, 16, 17, 18 });
				pBlackboard->setInt("iRandomIndex", iRandomHitIndex);

				m_pTransformCom->Look_At(vTargetPos);
				Transition(iRandomHitIndex, 1.7f);
				pBlackboard->setBool("Is_OneTick", false);
				
			}

			 if (true == m_pModelCom->Get_CurrentAnimation()->Get_Finished())
			 {
				 m_pModelCom->Set_Loop(true);
				 pBlackboard->setBool("Is_Hit", false);
				 pBlackboard->setBool("Is_OneTick", true);
				return BT_STATUS::Success;
			 }
			 else
				return BT_STATUS::Running;
		 }
		 else
		 {
			pBlackboard->setBool("Is_OneTick", true);
			return BT_STATUS::Failure;
	     }
			
			
	 };

	 FUNCTION_NODE Task_IsHit
	 = FUNCTION_NODE_MAKE
	 {
		if (0 >= pBlackboard->getFloat("Current_HP"))
		{
			pBlackboard->setBool("Is_Dead", true);
			return BT_STATUS::Success;
		}
		else
			return BT_STATUS::Failure;
	 };

	 FUNCTION_NODE Control_InAttackRange
		 = FUNCTION_NODE_MAKE
	 {
		   _vector vTargetPosition = XMLoadFloat3(&pBlackboard->getFloat3("TargetPosition"));
			vTargetPosition.m128_f32[3] = 1.f;
		   _vector vPosition = XMLoadFloat3(&pBlackboard->getFloat3("MyPosition"));
		   vPosition.m128_f32[3] = 1.f;

		   if (InRange(XMVectorGetX(XMVector3Length(vPosition - vTargetPosition)), 0.f, 3.f, "[]"))
		   {
				return BT_STATUS::Success;
		   }
		   else
			   return BT_STATUS::Failure;
	 };

	 FUNCTION_NODE Control_RandomTry
		= FUNCTION_NODE_MAKE
	 {
		_bool bRandomTry = Random({true, false, true, false, false, true, true, false, true, false});
		
		_vector vTargetPosition = XMLoadFloat3(&pBlackboard->getFloat3("TargetPosition"));
		vTargetPosition.m128_f32[3] = 1.f;
	 
		m_pTransformCom->Look_At(vTargetPosition);


		if(true == bRandomTry)
			return BT_STATUS::Success;

		else 
			return BT_STATUS::Failure;
	 };

	
	 
	 FUNCTION_NODE Task_LeftKeepEyeReady
		= FUNCTION_NODE_MAKE
	 {
		m_pModelCom->Set_Loop(false);
		Transition(13);

		pBlackboard->setInt("iDirState", 1); //! 0은 정면, 1은 좌측, 2는 우측

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			m_pModelCom->Set_Loop(true);
			m_pModelCom->Get_CurrentAnimation()->Reset_Animation();
			return BT_STATUS::Success;
		}
		else if (true == pBlackboard->getBool("Is_Hit") || 0 >= pBlackboard->getFloat("Current_HP"))
			return BT_STATUS::Failure;
		else
			return BT_STATUS::Running;
		
	 };

	 FUNCTION_NODE Task_LeftKeepEye
		= FUNCTION_NODE_MAKE
	 {
		m_pModelCom->Set_Loop(false);
		Transition(3);

		if (true == pBlackboard->getBool("Is_Hit") || 0 >= pBlackboard->getFloat("Current_HP"))
			return BT_STATUS::Failure;

		m_pTransformCom->KeepEye(fTimeDelta, false, pBlackboard->GetNavigation());

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			m_pModelCom->Set_Loop(true);
			m_pModelCom->Get_CurrentAnimation()->Reset_Animation();
			return BT_STATUS::Success;
		}
		else
			return BT_STATUS::Running;
		
	 };

	 FUNCTION_NODE Task_LeftFrontTurn
		 = FUNCTION_NODE_MAKE
	 {
		 m_pModelCom->Set_Loop(false);
		 pBlackboard->setInt("iDirState", 0); //! 0은 정면, 1은 좌측, 2는 우측
		 Transition(14);

		 if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		 {
			 m_pModelCom->Set_Loop(true);
			 m_pModelCom->Get_CurrentAnimation()->Reset_Animation();
			 return BT_STATUS::Success;
		 }
		 else if (true == pBlackboard->getBool("Is_Hit") || 0 >= pBlackboard->getFloat("Current_HP"))
			 return BT_STATUS::Failure;
		 else
			 return BT_STATUS::Running;
	 };

	 //! 우측 간보기 2
	//! 우측 간보기 준비 11
	//! 우측 에서 앞으로 돌진 준비 12
	//! 좌측 간보기 3
	//! 좌측 간보기 준비 13
	//! 좌측에서 앞으로 돌진 준비 14
	//! 뒤로 간보기 4
	//! 
	 FUNCTION_NODE Task_RightKeepEyeReady
		 = FUNCTION_NODE_MAKE
	 {
		m_pModelCom->Set_Loop(false);
		Transition(11);
		pBlackboard->setInt("iDirState", 2); //! 0은 정면, 1은 좌측, 2는 우측
		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			m_pModelCom->Set_Loop(true);
			m_pModelCom->Get_CurrentAnimation()->Reset_Animation();
			return BT_STATUS::Success;
		}
		else if (true == pBlackboard->getBool("Is_Hit") || 0 >= pBlackboard->getFloat("Current_HP"))
			return BT_STATUS::Failure;
		else
			return BT_STATUS::Running;

	 };

	 FUNCTION_NODE Task_RightKeepEye
		 = FUNCTION_NODE_MAKE
	 {
		m_pModelCom->Set_Loop(false);
		Transition(2);

		if (true == pBlackboard->getBool("Is_Hit") || 0 >= pBlackboard->getFloat("Current_HP"))
			return BT_STATUS::Failure;

		m_pTransformCom->KeepEye(fTimeDelta, pBlackboard->GetNavigation());

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			m_pModelCom->Set_Loop(true);
			m_pModelCom->Get_CurrentAnimation()->Reset_Animation();
			return BT_STATUS::Success;
		}
		else
			return BT_STATUS::Running;
		
	 };

	 FUNCTION_NODE Task_RightFrontTurn
		= FUNCTION_NODE_MAKE
	 {
		 m_pModelCom->Set_Loop(false);
		 Transition(12);
		 pBlackboard->setInt("iDirState", 0); //! 0은 정면, 1은 좌측, 2는 우측

		 if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		 {
			 m_pModelCom->Set_Loop(true);
			 m_pModelCom->Get_CurrentAnimation()->Reset_Animation();
			 return BT_STATUS::Success;
		 }
		 else if (true == pBlackboard->getBool("Is_Hit") || 0 >= pBlackboard->getFloat("Current_HP"))
			 return BT_STATUS::Failure;
		 else
			 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_FrontKickAttack
		= FUNCTION_NODE_MAKE
	 {
		Transition(55);
		m_pModelCom->Set_Loop(false);
		m_pColliderCom->OnAttackBody();

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			m_pModelCom->Set_Loop(true);
			m_pColliderCom->OffAttackBody();
			 return BT_STATUS::Success;
		}
		 else if (true == pBlackboard->getBool("Is_Hit") || 0 >= pBlackboard->getFloat("Current_HP"))
			 return BT_STATUS::Failure;
		 else
			 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_JumpAttack1
		= FUNCTION_NODE_MAKE
	 {
		 Transition(5);
		 m_pModelCom->Set_Loop(false);

		 if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		 {
			 m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Success;
		 }
		 else if (true == pBlackboard->getBool("Is_Hit") || 0 >= pBlackboard->getFloat("Current_HP"))
			 return BT_STATUS::Failure;
		 else
			 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_JumpAttack2
		 = FUNCTION_NODE_MAKE
	 {
		 Transition(6);
		 m_pModelCom->Set_Loop(false);

		 _vector vTargetPosition = XMLoadFloat3(&pBlackboard->getFloat3("TargetPosition"));
		 vTargetPosition.m128_f32[3] = 1.f;

		 m_pTransformCom->Set_SpeedPerSec(8.f);
		 
		 m_pTransformCom->Go_Target_Navi(vTargetPosition, fTimeDelta, pBlackboard->GetNavigation());

		 if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		 {
			 
			 m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Success;
		 }
		 else if (true == pBlackboard->getBool("Is_Hit") || 0 >= pBlackboard->getFloat("Current_HP"))
			 return BT_STATUS::Failure;
		 else
			 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_JumpAttack3
		 = FUNCTION_NODE_MAKE
	 {
		 Transition(8);
		 m_pModelCom->Set_Loop(false);

		 _vector vTargetPosition = XMLoadFloat3(&pBlackboard->getFloat3("TargetPosition"));
		 vTargetPosition.m128_f32[3] = 1.f;

		 m_pTransformCom->Go_Target_Navi(vTargetPosition, fTimeDelta, pBlackboard->GetNavigation());

		 m_pColliderCom->OnAttackBody();

		 if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		 {
			 m_pModelCom->Set_Loop(true);
			 m_pColliderCom->OffAttackBody();
			 m_pTransformCom->Set_SpeedPerSec(3.f);
			 return BT_STATUS::Success;
		 }
		 else if (true == pBlackboard->getBool("Is_Hit") || 0 >= pBlackboard->getFloat("Current_HP"))
			 return BT_STATUS::Failure;
		 else
			 return BT_STATUS::Running;
	 };


	 FUNCTION_NODE Control_DashRush
		= FUNCTION_NODE_MAKE
	 {
		_vector vTargetPosition = XMLoadFloat3(&pBlackboard->getFloat3("TargetPosition"));
		   _vector vPosition = XMLoadFloat3(&pBlackboard->getFloat3("MyPosition"));

		   vTargetPosition.m128_f32[3] = 1.f;
		   vPosition.m128_f32[3] = 1.f;
		   m_pModelCom->Set_Loop(true);
		   
		   m_pTransformCom->Set_SpeedPerSec(5.f);
		   m_pTransformCom->Set_RotationPerSec(XMConvertToRadians(110.f));
		   //_vector vDir = XMLoadFloat3(&pBlackboard->getFloat3("TargetPosition")) - XMLoadFloat3(&pBlackboard->getFloat3("MyPosition"));
		   //_float fDistance = XMVectorGetX(XMVector3Length(vDir));

		   if (InRange(XMVectorGetX(XMVector3Length(vPosition - vTargetPosition)), 0.f, 12.0f, "[]"))
		   {
			   Transition(60);
			   m_pTransformCom->Set_SpeedPerSec(3.f);
			   m_pTransformCom->Set_RotationPerSec(XMConvertToRadians(90.f));
			  return BT_STATUS::Success;
		   }
		   else
			   return BT_STATUS::Failure;
	 };

	 FUNCTION_NODE Task_DashRush
		= FUNCTION_NODE_MAKE
	 {
		if (true == pBlackboard->getBool("Is_Hit") || 0 >= pBlackboard->getFloat("Current_HP"))
			 return BT_STATUS::Success;

		_vector vTargetPosition = XMLoadFloat3(&pBlackboard->getFloat3("TargetPosition"));
		vTargetPosition.m128_f32[3] = 1.f;

		m_pTransformCom->Go_Target_Navi(vTargetPosition, fTimeDelta, pBlackboard->GetNavigation());

		return BT_STATUS::Success;
	 };

	 FUNCTION_NODE Control_InChaseRange
	 = FUNCTION_NODE_MAKE
	 {		
		   _vector vTargetPosition = XMLoadFloat3(&pBlackboard->getFloat3("TargetPosition"));
		   _vector vPosition = XMLoadFloat3(&pBlackboard->getFloat3("MyPosition"));

		   vTargetPosition.m128_f32[3] = 1.f;
		   vPosition.m128_f32[3] = 1.f;
		   m_pModelCom->Set_Loop(true);
		   
		 //_vector vDir = XMLoadFloat3(&pBlackboard->getFloat3("TargetPosition")) - XMLoadFloat3(&pBlackboard->getFloat3("MyPosition"));
		 //_float fDistance = XMVectorGetX(XMVector3Length(vDir));

		 if (InRange(XMVectorGetX(XMVector3Length(vPosition - vTargetPosition)), 0.f, 12.0f, "[]"))
		 {
			Transition(3);
			return BT_STATUS::Success;
		 }
		 else
			 return BT_STATUS::Failure;
	 };

	 FUNCTION_NODE Task_InChaseRange
		 = FUNCTION_NODE_MAKE
	 {

		 if (true == pBlackboard->getBool("Is_Hit") || 0 >= pBlackboard->getFloat("Current_HP"))
			 return BT_STATUS::Success;

		_vector vTargetPosition = XMLoadFloat3(&pBlackboard->getFloat3("TargetPosition"));
		vTargetPosition.m128_f32[3] = 1.f;

		m_pTransformCom->Go_Target_Navi(vTargetPosition, fTimeDelta, pBlackboard->GetNavigation());
			
		return BT_STATUS::Success;
	 };

	 FUNCTION_NODE Control_PlayerIsDead
		= FUNCTION_NODE_MAKE
	 {
		 if(pBlackboard->GetTarget()->Is_Dead())
			return BT_STATUS::Success;
		 else
			return BT_STATUS::Failure;
	 };

	 FUNCTION_NODE Task_Idle
		= FUNCTION_NODE_MAKE
	 {

	  if (true == pBlackboard->getBool("Is_Hit") || 0 >= pBlackboard->getFloat("Current_HP"))
			 return BT_STATUS::Success;

		 Transition(0);

		 return BT_STATUS::Success;
	 };




	 m_pBehaviorTree = Builder(EN00_BlackBoard)
		.composite<Selector>()
			.composite<Sequence>()
				.leaf<FunctionNode>(Control_IsDead)
				.leaf<FunctionNode>(Task_IsDead)
			.end()
			.composite<Sequence>()
				.leaf<FunctionNode>(Control_IsHit)
				.leaf<FunctionNode>(Task_IsHit)
			.end()
			.composite<Sequence>()
				.composite<Sequence>()
					.leaf<FunctionNode>(Control_InAttackRange)
					.composite<Selector>()
						.composite<Sequence>()
						.leaf<FunctionNode>(Control_RandomTry)
						.leaf<FunctionNode>(Task_FrontKickAttack)
						.end()
						.composite<Sequence>()
						.leaf<FunctionNode>(Control_RandomTry)
						.leaf<FunctionNode>(Task_JumpAttack1)
						.leaf<FunctionNode>(Task_JumpAttack2)
						.leaf<FunctionNode>(Task_JumpAttack3)
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Control_RandomTry)
							.leaf<FunctionNode>(Task_LeftKeepEyeReady)
							.leaf<FunctionNode>(Task_LeftKeepEye)
							.leaf<FunctionNode>(Task_LeftFrontTurn)
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Control_RandomTry)
							.leaf<FunctionNode>(Task_RightKeepEyeReady)
							.leaf<FunctionNode>(Task_RightKeepEye)
							.leaf<FunctionNode>(Task_RightFrontTurn)
						.end()
					.end()
				.end()
			.end()
			.composite<Sequence>()
				.leaf<FunctionNode>(Control_DashRush)
				.leaf<FunctionNode>(Task_DashRush)
			.end()
			.leaf<FunctionNode>(Task_Idle)
		.end()
		.build();
	
	
	return S_OK;
}



HRESULT CMonster_EN70::Add_PartObject(const wstring& strPrototypeTag, const wstring& strPartTag, void* pArg)
{
	if (nullptr != Find_PartObject(strPrototypeTag))
		return E_FAIL;

	CGameObject* pPartObject = m_pGameInstance->Get_CloneObject(strPrototypeTag, pArg);

	if (nullptr == pPartObject)
		return E_FAIL;

	m_PartObjects.emplace(strPartTag, dynamic_cast<CPartObject*>(pPartObject));

	return S_OK;
}

HRESULT CMonster_EN70::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	
	return S_OK;
}


CMonster_EN70* CMonster_EN70::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CMonster_EN70* pInstance = new CMonster_EN70(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CMonster_EN70");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMonster_EN70::Clone(void* pArg)
{
	CMonster_EN70* pInstance = new CMonster_EN70(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster_EN70");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonster_EN70::Free()
{
	__super::Free();

	for (auto& Pair : m_PartObjects)
		Safe_Release(Pair.second);

		m_PartObjects.clear();
}
