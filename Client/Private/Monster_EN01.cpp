#include "stdafx.h"
#include "Monster_EN01.h"
#include "GameInstance.h"

#include "PartObject.h"
#include "Bone.h"
#include "Animation.h"
#include "MonsterPart_EN01_Weapon.h"



CMonster_EN01::CMonster_EN01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice,pContext)
{
}

CMonster_EN01::CMonster_EN01(const CMonster_EN01& rhs)
	: CMonster(rhs)
{
}

CPartObject* CMonster_EN01::Find_PartObject(const wstring& strPartTag)
{
	auto iter = m_PartObjects.find(strPartTag);

	if (iter == m_PartObjects.end())
		return nullptr;

	return iter->second;
}

HRESULT CMonster_EN01::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelID = eLevel;
	
	return S_OK;
}

HRESULT CMonster_EN01::Initialize(void* pArg)
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

	Init_Status(80.f, 0.f);

	return S_OK;
}

void CMonster_EN01::Priority_Tick(_float fTimeDelta)
{

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Priority_Tick(fTimeDelta);
	}
}

void CMonster_EN01::Tick(_float fTimeDelta)
{
	if (m_eCurrentLevelID != LEVEL_TOOL)
	{
		Blackboard::Ptr pBlackBoard = m_pBehaviorTree->getBlackboard();
		
		_float3 vMyPos, vTargetPos;
		XMStoreFloat3(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		XMStoreFloat3(&vTargetPos, pBlackBoard->GetTarget()->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		
		pBlackBoard->setFloat3("MyPosition", vMyPos);
		pBlackBoard->setFloat3("TargetPosition", vTargetPos);
		pBlackBoard->setFloat("CurrentTrackPosition", m_pModelCom->Get_CurrentAnimation()->Get_TrackPosition());
		

		

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

	if(m_eCurrentLevelID != LEVEL_TOOL)
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

void CMonster_EN01::Late_Tick(_float fTimeDelta)
{

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Late_Tick(fTimeDelta);
	}


	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this)))
		return;
}

HRESULT CMonster_EN01::Render()
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

HRESULT CMonster_EN01::Render_Shadow()
{
	
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	//#몬스터모델렌더
	_float4x4		ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(-20.f, 100.f, -20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), g_iWinSizeX / (float)g_iWinSizeY, 0.1f, 600.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	//TODO 클라에서 모델의 메시 개수를 받아와서 순회하면서 셰이더 바인딩해주자.

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		m_pShaderCom->Begin(2); //! 셰이더에 던져주고 비긴 호출하는 걸 잊지말자

		m_pModelCom->Render(i);
	}


	return S_OK;
}

void CMonster_EN01::Write_Json(json& Out_Json)
{
	__super::Write_Json(Out_Json);
	
}

void CMonster_EN01::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);

}

void CMonster_EN01::Init_Desc()
{

}

void CMonster_EN01::On_Collision(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit)
{
	__super::On_Collision(pCollisionObject, LeftTag, RightTag, vCollisionPos, bType, bHit);

	//fif (RightTag == TEXT("Layer_Player"))
	//f{
	//f	_float magnitude = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vCollisionPos)));
	//f
	//f
	//f	_vector vLook = pCollisionObject->Get_Transform()->Get_State(CTransform::STATE_LOOK);
	//f	_vector vForceDir = vLook * magnitude;
	//f
	//f	_float3 vCalcPos;
	//f	XMStoreFloat3(&vCalcPos, vForceDir);
	//f
	//f	m_pRigidBodyCom->Add_Force(vCalcPos, CRigidBody::FORCE_MODE::FORCE);
	//f}

	if (RightTag == TEXT("Layer_Monster") && false == bHit)
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

void CMonster_EN01::On_CollisionEnter(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit)
{
	

	
	__super::On_CollisionEnter(pCollisionObject, LeftTag, RightTag, vCollisionPos, bType, bHit);
}

void CMonster_EN01::On_CollisionExit(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit)
{
	__super::On_CollisionExit(pCollisionObject, LeftTag, RightTag, bType, bHit);

	m_pRigidBodyCom->Clear_NetPower();
}

HRESULT CMonster_EN01::Ready_Components()
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
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Model_Monster_EN01"),
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

HRESULT CMonster_EN01::Ready_PartObjects()
{
	CPartObject::PART_DESC WeaponDesc = {};

	
	
	CBone* pSwordBone = m_pModelCom->Get_BonePtr("M_WP2");

	if(nullptr == pSwordBone)
		return E_FAIL;

	WeaponDesc.m_pSocketBone = pSwordBone;
	WeaponDesc.m_pParentTransform = m_pTransformCom;


	if(FAILED(Add_PartObject(TEXT("Prototype_PartObject_Monster_EN01_Weapon"), TEXT("Part_Weapon"), &WeaponDesc)))
		return E_FAIL;

	return S_OK;
}



HRESULT CMonster_EN01::Ready_BehaviorTree_V2()
{
	m_pBehaviorTree = std::make_shared<BrainTree::BehaviorTree>();

	 BrainTree::Blackboard::Ptr EN01_BlackBoard = m_pBehaviorTree->getBlackboard();
	 	
	 EN01_BlackBoard->setString("Name", "EN00");
	 EN01_BlackBoard->setFloat("Max_HP", 80.f);
	 EN01_BlackBoard->setFloat("Current_HP", 80.f);
	 EN01_BlackBoard->setFloat("Attack_Range", 15.f);
	 EN01_BlackBoard->setFloat("Keep_Range", 3.5f);
	 EN01_BlackBoard->setFloat("Detect_Range", 20.f);
	 EN01_BlackBoard->setFloat("Speed", m_pTransformCom->Get_Speed());
	 EN01_BlackBoard->setFloat("RotateSpeed", m_pTransformCom->Get_RotationSpeed());


	 CGameObject* pTarget = nullptr;

	 pTarget = m_pGameInstance->Get_Player(m_eCurrentLevelID);

	 if (nullptr != pTarget)
	 {
		 EN01_BlackBoard->setTarget(pTarget);
	 }

	 _float3 vMyPos, vTargetPos;
	 
	 XMStoreFloat3(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	 XMStoreFloat3(&vTargetPos, pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION));

	 EN01_BlackBoard->setFloat3("MyPosition", vMyPos);
	 EN01_BlackBoard->setFloat3("TargetPosition", vTargetPos);
	 EN01_BlackBoard->setFloat("CurrentTrackPosition", 0.f);

	 //EN01_BlackBoard->setFloat3("TargetPostion", _float3(0.f, 0.f, 0.f));
	 EN01_BlackBoard->setBool("Is_Shot", false);
	 EN01_BlackBoard->setBool("Is_Hit", false); //! 피격중인가
	 EN01_BlackBoard->setBool("Is_Ground", true);
	 EN01_BlackBoard->setBool("Is_Dead", false);
	 //EN01_BlackBoard->setBool("Is_TargetPosition", false); //! 목표로 하는 지점이 있는가
	 EN01_BlackBoard->setBool("Is_Patrol", false); //! 순찰해야하는가?
	 EN01_BlackBoard->setBool("Is_OneTick", true);
	 EN01_BlackBoard->setInt("iRandomIndex", -1);
	 EN01_BlackBoard->setOwner(this);
	 EN01_BlackBoard->setModel(m_pModelCom);
	 EN01_BlackBoard->setShader(m_pShaderCom);
	 EN01_BlackBoard->setCollider(m_pColliderCom);
	 EN01_BlackBoard->setTransform(m_pTransformCom);
	 EN01_BlackBoard->setGameInstance(m_pGameInstance);
	 EN01_BlackBoard->setNavigation(m_pNavigationCom);
	 EN01_BlackBoard->setRigidBody(m_pRigidBodyCom);
	 
	 
	 
	 FUNCTION_NODE Control_IsDead
	 = FUNCTION_NODE_MAKE
	 {
		 if (true == pBlackboard->getBool("Is_Dead"))
		 {
			m_pModelCom->Set_Loop(false);
			Transition(31);
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
				_int iRandomHitIndex = Random({ 19, 20, 21, 22, 23 });
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
			 else if (true == pBlackboard->getBool("Is_Dead"))
			 {
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

		   if (InRange(XMVectorGetX(XMVector3Length(vPosition - vTargetPosition)), 0.f, pBlackboard->getFloat("Attack_Range"), "[]"))
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


	 FUNCTION_NODE Task_NormalShot
		 = FUNCTION_NODE_MAKE
	 {
		Transition(6);
		
		_vector vTargetPos = XMLoadFloat3(&Get_TargetPosition());
		vTargetPos.m128_f32[3] = 1.f;

		m_pTransformCom->TurnToTarget(vTargetPos, fTimeDelta);

		 m_pModelCom->Set_Loop(false);

		 if (101.f < pBlackboard->getFloat("CurrentTrackPosition") && false == pBlackboard->getBool("Is_Shot"))
		 {
			 pBlackboard->setBool("Is_Shot", true);
			 ChargeNormalShot();
		 }

		 if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		 {
			 m_pModelCom->Set_Loop(true);
			 pBlackboard->setBool("Is_Shot", false);
			 m_pModelCom->Get_CurrentAnimation()->Reset_Animation();
			 return BT_STATUS::Success;
		 }
		else if(true == pBlackboard->getBool("Is_Hit") || 0 >= pBlackboard->getFloat("Current_HP"))
			return BT_STATUS::Failure;
		else
			return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_NormalShotEnd
		 = FUNCTION_NODE_MAKE
	 {
		_vector vTargetPos = XMLoadFloat3(&Get_TargetPosition());
		vTargetPos.m128_f32[3] = 1.f;

		m_pTransformCom->TurnToTarget(vTargetPos, fTimeDelta);
		Transition(7);
		m_pModelCom->Set_Loop(false);

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

	 FUNCTION_NODE Task_ParabolicShot1
		 = FUNCTION_NODE_MAKE
	 {
		_vector vTargetPos = XMLoadFloat3(&Get_TargetPosition());
		vTargetPos.m128_f32[3] = 1.f;

		m_pTransformCom->TurnToTarget(vTargetPos, fTimeDelta);

		Transition(8);
		m_pModelCom->Set_Loop(false);

		if (99.f < pBlackboard->getFloat("CurrentTrackPosition") && false == pBlackboard->getBool("Is_Shot"))
		{
			pBlackboard->setBool("Is_Shot", true);
			ParabolicShot();
		}

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			m_pModelCom->Set_Loop(true);
			pBlackboard->setBool("Is_Shot", false);
			 return BT_STATUS::Success;
		}
		 else if (true == pBlackboard->getBool("Is_Hit") || 0 >= pBlackboard->getFloat("Current_HP"))
			 return BT_STATUS::Failure;
		 else
			 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_ParabolicShot2
		 = FUNCTION_NODE_MAKE
	 {
		_vector vTargetPos = XMLoadFloat3(&Get_TargetPosition());
		vTargetPos.m128_f32[3] = 1.f;

		m_pTransformCom->TurnToTarget(vTargetPos, fTimeDelta);

		Transition(9);
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

	 FUNCTION_NODE Task_ChargeNormalShot
		 = FUNCTION_NODE_MAKE
	 {
		_vector vTargetPos = XMLoadFloat3(&Get_TargetPosition());
		vTargetPos.m128_f32[3] = 1.f;

		m_pTransformCom->TurnToTarget(vTargetPos, fTimeDelta);
		Transition(10);
		m_pModelCom->Set_Loop(false);

		if (147.f < pBlackboard->getFloat("CurrentTrackPosition") && false == pBlackboard->getBool("Is_Shot"))
		{
			pBlackboard->setBool("Is_Shot", true);
			ChargeNormalShot();
		}


		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			pBlackboard->setBool("Is_Shot", false);
			m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Success;
		}
		 else if (true == pBlackboard->getBool("Is_Hit") || 0 >= pBlackboard->getFloat("Current_HP"))
			 return BT_STATUS::Failure;
		 else
			 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_ChargeParabolicShot
		 = FUNCTION_NODE_MAKE
	 {
		_vector vTargetPos = XMLoadFloat3(&Get_TargetPosition());
		vTargetPos.m128_f32[3] = 1.f;

		m_pTransformCom->TurnToTarget(vTargetPos, fTimeDelta);
		Transition(11);
		m_pModelCom->Set_Loop(false);

		if (147.f < pBlackboard->getFloat("CurrentTrackPosition") && false == pBlackboard->getBool("Is_Shot"))
		{
			pBlackboard->setBool("Is_Shot", true);
			ChargeParabolicShot();
		}
		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			pBlackboard->setBool("Is_Shot", false);
			m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Success;
		}
		 else if (true == pBlackboard->getBool("Is_Hit") || 0 >= pBlackboard->getFloat("Current_HP"))
			 return BT_STATUS::Failure;
		 else
			 return BT_STATUS::Running;
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

		 if (InRange(XMVectorGetX(XMVector3Length(vPosition - vTargetPosition)), 0.f, pBlackboard->getFloat("Detect_Range"), "[]"))
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

		 Transition(0);

		 return BT_STATUS::Success;
	 };




	 m_pBehaviorTree = Builder(EN01_BlackBoard)
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
							.leaf<FunctionNode>(Task_NormalShot)
							.leaf<FunctionNode>(Task_NormalShotEnd)
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Control_RandomTry)
							.leaf<FunctionNode>(Task_ParabolicShot1)
							.leaf<FunctionNode>(Task_ParabolicShot2)
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Control_RandomTry)
							.leaf<FunctionNode>(Task_ChargeNormalShot)
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Control_RandomTry)
							.leaf<FunctionNode>(Task_ChargeParabolicShot)
						.end()
					.end()
				.end()
			.end()
			.composite<Sequence>()
				.leaf<FunctionNode>(Control_InChaseRange)
				.leaf<FunctionNode>(Task_InChaseRange)
			.end()
			.leaf<FunctionNode>(Task_Idle)
		.end()
	.build();
	
	
	return S_OK;
}



HRESULT CMonster_EN01::Add_PartObject(const wstring& strPrototypeTag, const wstring& strPartTag, void* pArg)
{
	if (nullptr != Find_PartObject(strPrototypeTag))
		return E_FAIL;

	CGameObject* pPartObject = m_pGameInstance->Get_CloneObject(strPrototypeTag, pArg);

	if (nullptr == pPartObject)
		return E_FAIL;

	m_PartObjects.emplace(strPartTag, dynamic_cast<CPartObject*>(pPartObject));

	return S_OK;
}

HRESULT CMonster_EN01::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	
	return S_OK;
}

void CMonster_EN01::NormalShot()
{
	CMonsterPart_EN01_Weapon* pBow = dynamic_cast<CMonsterPart_EN01_Weapon*>(Find_PartObject(TEXT("Part_Weapon")));

	CGameObject* pArrow = pBow->CreateNormalArrow();

	//CCollider* pCollider = dynamic_cast<CCollider*>(pArrow->Find_Component(TEXT("Com_Collider")));
	//
	//m_vecColliders.push_back(pCollider);
}

void CMonster_EN01::ParabolicShot()
{
	CMonsterPart_EN01_Weapon* pBow = dynamic_cast<CMonsterPart_EN01_Weapon*>(Find_PartObject(TEXT("Part_Weapon")));

	CGameObject* pArrow = pBow->CreateParabolicArrow();

	//CCollider* pCollider = dynamic_cast<CCollider*>(pArrow->Find_Component(TEXT("Com_Collider")));
	//
	//m_vecColliders.push_back(pCollider);
	//
	//m_vecArrows.push_back(pArrow);
}

void CMonster_EN01::ChargeNormalShot()
{
	CMonsterPart_EN01_Weapon* pBow = dynamic_cast<CMonsterPart_EN01_Weapon*>(Find_PartObject(TEXT("Part_Weapon")));

	CGameObject* pArrow = pBow->CreateChargeNormalArrow();

	//CCollider* pCollider = dynamic_cast<CCollider*>(pArrow->Find_Component(TEXT("Com_Collider")));
	//
	//m_vecColliders.push_back(pCollider);
	//
	//m_vecArrows.push_back(pArrow);
}

void CMonster_EN01::ChargeParabolicShot()
{
	CMonsterPart_EN01_Weapon* pBow = dynamic_cast<CMonsterPart_EN01_Weapon*>(Find_PartObject(TEXT("Part_Weapon")));

	CGameObject* pArrow = pBow->CreateChargeParabolicArrow();

//	CCollider* pCollider = dynamic_cast<CCollider*>(pArrow->Find_Component(TEXT("Com_Collider")));
//
//	m_vecColliders.push_back(pCollider);

}




CMonster_EN01* CMonster_EN01::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CMonster_EN01* pInstance = new CMonster_EN01(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CMonster_EN01");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMonster_EN01::Clone(void* pArg)
{
	CMonster_EN01* pInstance = new CMonster_EN01(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster_EN01");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonster_EN01::Free()
{
	__super::Free();

	for (auto& Pair : m_PartObjects)
		Safe_Release(Pair.second);

		m_PartObjects.clear();
}
