#include "stdafx.h"
#include "Monster_EN70.h"
#include "GameInstance.h"

#include "PartObject.h"
#include "Bone.h"
#include "Animation.h"
#include "MonsterPart_EN70_Weapon.h"


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


	Init_Status(500.f, 30.f);

	if (FAILED(__super::Initialize_UI(MONSTERTYPE::EN70)))
		return E_FAIL;
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
		pBlackBoard->setFloat("CurrentTrackPosition", m_pModelCom->Get_CurrentAnimation()->Get_TrackPosition());

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
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this)))
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

		m_pShaderCom->Begin(m_iPassIndex);

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG

	m_pColliderCom->Render();

#endif

	return S_OK;
}

HRESULT CMonster_EN70::Render_Shadow()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(-20.f, 20.f, -20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), g_iWinSizeX / (float)g_iWinSizeY, 0.1f, 3000.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR);

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}

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

void CMonster_EN70::On_CollisionEnter(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit)
{
	//if (RightTag == TEXT("Layer_Player"))
	//{
	//	_float magnitude = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vCollisionPos)));
	//
	//
	//	_vector vLook = pCollisionObject->Get_Transform()->Get_State(CTransform::STATE_LOOK);
	//	_vector vForceDir = vLook * magnitude;
	//
	//	_float3 vCalcPos;
	//	XMStoreFloat3(&vCalcPos, vForceDir);
	//
	//	m_pRigidBodyCom->Add_Force(vCalcPos, CRigidBody::FORCE_MODE::FORCE);
	//}

	if (true == m_pColliderCom->isAttackBody() && RightTag == TEXT("Layer_Player"))
	{
		CRigidBody* pTargetBody = dynamic_cast<CRigidBody*>(pCollisionObject->Find_Component(TEXT("Com_RigidBody")));


		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 500.f;

		_float3 vForce;
		XMStoreFloat3(&vForce, vLook);

		pTargetBody->Add_Force(vForce, CRigidBody::FORCE_MODE::IMPULSE);

	}
	
	if (RightTag == TEXT("Layer_Player") && bHit == true)
	{
		m_pGameInstance->Play_Sound(L"EN70_EFFECT", L"MonsterHit.wav", SOUND_EN70, 0.5f);
	}

	__super::On_CollisionEnter(pCollisionObject, LeftTag, RightTag,vCollisionPos, bType, bHit);
}

void CMonster_EN70::On_CollisionExit(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit)
{
	__super::On_CollisionExit(pCollisionObject, LeftTag, RightTag, bType, bHit);

	m_pRigidBodyCom->Clear_NetPower();

	if (true == m_pColliderCom->isAttackBody() && LeftTag == TEXT("Layer_Player"))
	{
		CRigidBody* pTargetBody = dynamic_cast<CRigidBody*>(pCollisionObject->Find_Component(TEXT("Com_RigidBody")));

		//pTargetBody->Clear_Force(CRigidBody::FORCE_MODE::IMPULSE);

	}
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

// 	/* For.Com_Collider */
// 	CBoundingBox_OBB::BOUNDING_OBB_DESC BoundingDesc = {};
// 
// 	BoundingDesc.vExtents = _float3(0.7f, 0.7f, 0.7f);
// 	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
// 	BoundingDesc.ePartType = CBoundParent::PARTTYPE_BOUND::PART_BODY;
// 
// 	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Collider_OBB"),
// 		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
// 		return E_FAIL;

	CBoundingBox_AABB::BOUNDING_AABB_DESC		BoundingDesc = {};

	BoundingDesc.vExtents = _float3(0.7f, 0.7f, 0.7f);
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
	
	//! For.Com_Texture
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Texture_BossFireNoise"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDissoveTexture))))
		return E_FAIL;


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

	 BrainTree::Blackboard::Ptr EN70_BlackBoard = m_pBehaviorTree->getBlackboard();
	 	
	 EN70_BlackBoard->setString("Name", "EN70");
	 EN70_BlackBoard->setFloat("Max_HP", 500.f);
	 EN70_BlackBoard->setFloat("Current_HP", 500.f);
	 EN70_BlackBoard->setFloat("Attack_Range", 3.f);
	 EN70_BlackBoard->setFloat("Keep_Range", 3.5f);
	 EN70_BlackBoard->setFloat("Detect_Range", 10.f);
	 EN70_BlackBoard->setFloat("Speed", m_pTransformCom->Get_Speed());
	 EN70_BlackBoard->setFloat("RotateSpeed", m_pTransformCom->Get_RotationSpeed());
	 EN70_BlackBoard->setBool("Is_Swing", false);
	 EN70_BlackBoard->setFloat("CurrentTrackPosition", 0.f);

	 CGameObject* pTarget = nullptr;

	 pTarget = m_pGameInstance->Get_Player(m_eCurrentLevelID);

	 if (nullptr != pTarget)
	 {
		 EN70_BlackBoard->setTarget(pTarget);
	 }

	 _float3 vMyPos, vTargetPos;
	 
	 XMStoreFloat3(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	 XMStoreFloat3(&vTargetPos, pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION));


	 EN70_BlackBoard->setFloat3("MyPosition", vMyPos);
	 EN70_BlackBoard->setFloat3("TargetPosition", vTargetPos);
	 EN70_BlackBoard->setFloat3("DashRushTargetPosition", vTargetPos);
	 EN70_BlackBoard->setFloat3("EscapePosition", _float3());
	 EN70_BlackBoard->setFloat("DashEndTime", 3.f);
	 //EN70_BlackBoard->setFloat3("TargetPostion", _float3(0.f, 0.f, 0.f));
	 
	 EN70_BlackBoard->setBool("Is_Hit", false); //! 피격중인가
	 EN70_BlackBoard->setBool("Is_Ground", true);
	 EN70_BlackBoard->setBool("Is_Dead", false);
	 //EN70_BlackBoard->setBool("Is_TargetPosition", false); //! 목표로 하는 지점이 있는가
	 EN70_BlackBoard->setBool("Is_Patrol", false); //! 순찰해야하는가?
	 EN70_BlackBoard->setBool("Is_OneTick", true);
	 EN70_BlackBoard->setInt("iRandomIndex", -1);
	 EN70_BlackBoard->setOwner(this);
	 EN70_BlackBoard->setModel(m_pModelCom);
	 EN70_BlackBoard->setShader(m_pShaderCom);
	 EN70_BlackBoard->setCollider(m_pColliderCom);
	 EN70_BlackBoard->setTransform(m_pTransformCom);
	 EN70_BlackBoard->setGameInstance(m_pGameInstance);
	 EN70_BlackBoard->setNavigation(m_pNavigationCom);
	 EN70_BlackBoard->setRigidBody(m_pRigidBodyCom);
	 EN70_BlackBoard->setInt("iDirState", 0); //! 0은 정면, 1은 좌측, 2는 우측

	 
	
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
			Die(1.8f);
			Dead_Action(fTimeDelta, 1.0f);
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

		   if (InRange(XMVectorGetX(XMVector3Length(vPosition - vTargetPosition)), 0.f, 3.f, "[]"))
		   {
				return BT_STATUS::Success;
		   }
		   else if (true == pBlackboard->getBool("Is_Dead"))
		   {
			   return BT_STATUS::Failure;
		   }
		   else
			   return BT_STATUS::Failure;
	 };

	 FUNCTION_NODE Task_NormalSwingAttack1
		 = FUNCTION_NODE_MAKE
	 {
		
		  Transition(49);
		  On_Trail();

		  m_pModelCom->Set_Loop(false);
		  m_tStatus.eAttackType = tagStatusDesc::CHARGE_ATTACK;
		  
		  if (132.f < pBlackboard->getFloat("CurrentTrackPosition") && false == pBlackboard->getBool("Is_Swing"))
		  {
			  Get_WeaponCollider()->OnCollider();
				  
				  
				m_pGameInstance->Play_Sound(L"EN70_EFFECT", L"SwingAttack.wav", SOUND_EN70, 1.5f);

			  pBlackboard->setBool("Is_Swing", true);
		  }

		  if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		  {
			  Get_WeaponCollider()->OffCollider();
			  pBlackboard->setBool("Is_Swing", false);
			  m_pModelCom->Set_Loop(true);
			  m_pModelCom->Get_CurrentAnimation()->Reset_Animation();
			  return BT_STATUS::Success;
		  }
		  else if (true == pBlackboard->getBool("Is_Dead"))
		  {
			  return BT_STATUS::Failure;
		  }
		  else
			  return BT_STATUS::Running;
	 };
	 
	 FUNCTION_NODE Task_NormalSwingAttack2
		 = FUNCTION_NODE_MAKE
	 {
		Transition(50);
		m_pModelCom->Set_Loop(false);

		m_pModelCom->Root_MotionEnd();

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
		 m_pModelCom->Set_Loop(true);
		 m_pModelCom->Get_CurrentAnimation()->Reset_Animation();
		 return BT_STATUS::Success;
		}
		else if (true == pBlackboard->getBool("Is_Dead"))
		{
			return BT_STATUS::Failure;
		}
		else
		 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_NormalSwingAttack3
		 = FUNCTION_NODE_MAKE
	 {
		  Transition(51);

		  m_pModelCom->Set_Loop(false);
		  m_tStatus.eAttackType = tagStatusDesc::RUSH_ATTACK;

		  if (74.f < pBlackboard->getFloat("CurrentTrackPosition") && false == pBlackboard->getBool("Is_Swing"))
		  {
			  Get_WeaponCollider()->OnCollider();
			  m_pGameInstance->Play_Sound(L"EN70_EFFECT", L"SpearAttack.wav", SOUND_EN70, 1.5f);
			  pBlackboard->setBool("Is_Swing", true);
		  }

		  if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		  {
			  Get_WeaponCollider()->OffCollider();
			  pBlackboard->setBool("Is_Swing", false);
			  m_pModelCom->Set_Loop(true);
			  m_pModelCom->Get_CurrentAnimation()->Reset_Animation();
			  return BT_STATUS::Success;
		  }
		  else if (true == pBlackboard->getBool("Is_Dead"))
		  {
			  return BT_STATUS::Failure;
		  }
		  else
			  return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_NormalSwingAttack4
		 = FUNCTION_NODE_MAKE
	 {
		Transition(52);
		m_pModelCom->Set_Loop(false);
		

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
		 m_pModelCom->Set_Loop(true);
		 m_pModelCom->Get_CurrentAnimation()->Reset_Animation();
		 m_pModelCom->Root_MotionStart();
		 Off_Trail();
		 return BT_STATUS::Success;
		}
		else if (true == pBlackboard->getBool("Is_Dead"))
		{
			return BT_STATUS::Failure;
		}
		else
		 return BT_STATUS::Running;
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

	



	 FUNCTION_NODE Control_InChaseRange
	 = FUNCTION_NODE_MAKE
	 {		
		   _vector vTargetPosition = XMLoadFloat3(&pBlackboard->getFloat3("TargetPosition"));
		   _vector vPosition = XMLoadFloat3(&pBlackboard->getFloat3("MyPosition"));

		   vTargetPosition.m128_f32[3] = 1.f;
		   vPosition.m128_f32[3] = 1.f;
		   
		 if (InRange(XMVectorGetX(XMVector3Length(vPosition - vTargetPosition)), 0.f, 12.0f, "[]"))
		 {
			return BT_STATUS::Success;
		 }
		 else
		 {
			 m_pTransformCom->Set_SpeedPerSec(3.f);
			 m_pTransformCom->Set_RotationPerSec(XMConvertToRadians(75.f));
			return BT_STATUS::Failure;
		 }
	 };

	 FUNCTION_NODE Task_HorseDashReady
		= FUNCTION_NODE_MAKE
	 {
		Transition(59);

		m_pModelCom->Set_Loop(false);

		_vector vTargetPos = XMLoadFloat3(&Get_TargetPosition());
		_float3 vMyPos = Get_MyPosition();
		vTargetPos.m128_f32[3] = 1.f;
		
		if (m_bPlaySound == false)
		{
			m_pGameInstance->Play_Sound(L"EN70_EFFECT", L"DashReady.wav", SOUND_EN70, 1.5f);
			m_bPlaySound = true;
		}
		

		m_pTransformCom->TurnToTarget(vTargetPos, fTimeDelta);

		if (true == Is_CurrentAnimEnd())
		{
			pBlackboard->setFloat3("DashRushTargetPosition", Get_TargetPosition());

			_float4 vRandomPos = Create_RandomPosition(Get_MyPosition(), 10.f, 12.f);
			vRandomPos.y = vMyPos.y;
			pBlackboard->setFloat3("EscapePosition", _float3(vRandomPos.x, vRandomPos.y, vRandomPos.z));

			m_pModelCom->Set_Loop(true);
			m_bPlaySound = false;
			return BT_STATUS::Success;
		}
		else if (true == pBlackboard->getBool("Is_Dead"))
		{
			return BT_STATUS::Failure;
		}
		else
			return BT_STATUS::Running;
			
	 };

	 FUNCTION_NODE Control_DashRush
		 = FUNCTION_NODE_MAKE
	 {

		_bool bDashOrEscape;



		_int iRandom = Random({0, 1, 2, 3});

		_float3 vMyPos = Get_MyPosition();

		m_pTransformCom->Set_SpeedPerSec(13.f);
		m_pTransformCom->Set_RotationPerSec(XMConvertToRadians(75.f));
		

		if (iRandom == 0 || 2)
		{
			bDashOrEscape = true;
		}
		else
			bDashOrEscape = false;

		if (0.5f < Get_CurrentHpRatio())
		{
			return BT_STATUS::Success;
		}
		else if (true == bDashOrEscape)
			return BT_STATUS::Success;
		else
			return BT_STATUS::Failure;

	 };


	 FUNCTION_NODE Task_Dash
		 = FUNCTION_NODE_MAKE
	 {
		 Set_AnimSpeed(3.f);
		 Transition(60);
		 

		 if (m_bPlaySound == false)
		 {
			 m_pGameInstance->Play_Sound(L"EN70_EFFECT", L"Dash.wav", SOUND_EN70EFFECT1, 1.5f);
			 m_bPlaySound = true;
		 }

		 if (m_bPlaySound == true)
		 {
			 m_fDashTimeAcc += fTimeDelta;

			 if (m_fDashTimeAcc > m_fDashSoundTick)
			 {
				 m_bPlaySound = false;
				 m_fDashTimeAcc = 0.f;
			 }
		 }

		 m_tStatus.eAttackType = STATUS_DESC::RUSH_ATTACK;

		 Set_Move(false);
		 m_pColliderCom->OnAttackBody();
		 On_Trail();

		 _vector vMyPos = XMLoadFloat3(&Get_MyPosition());
		 _vector vDashPos = XMLoadFloat3(&Get_DashPosition());
		 vMyPos.m128_f32[3] = 1.f;
		 vDashPos.m128_f32[3] = 1.f;

		 m_pTransformCom->Go_Target_Navi(vDashPos, fTimeDelta, m_pNavigationCom);

		 if (InRange(XMVectorGetX(XMVector3Length(vMyPos - vDashPos)), 0.f, 0.5f, "[]"))
		 {
			 Reset_AnimSpeed();
			 m_pColliderCom->OffAttackBody();
			 Set_Move(true);
			 m_tStatus.eAttackType = STATUS_DESC::NORMAL_ATTACK;
			 return BT_STATUS::Success;
			 Off_Trail();
		 }
		 else
		 {
			if(false == m_pNavigationCom->isMove(vMyPos))
				return BT_STATUS::Failure;

			return BT_STATUS::Running;
		 }
	 };

	 FUNCTION_NODE Task_DashStop
		 = FUNCTION_NODE_MAKE
	 {
		
		 Transition(61);


		if (m_bPlaySound == false)
		{
			m_pGameInstance->Play_Sound(L"EN70_EFFECT", L"DashStop.wav", SOUND_EN70EFFECT1, 1.5f);
			m_bPlaySound = true;
		}


		
		m_pModelCom->Set_Loop(false);

		if (true == Is_CurrentAnimEnd())
		{
			return BT_STATUS::Success;
		}
		else
			return BT_STATUS::Running;

	 };


	 FUNCTION_NODE Task_Escape
		 = FUNCTION_NODE_MAKE
	 {
		Set_AnimSpeed(3.f);
		 m_pModelCom->Set_Loop(false);

		 Transition(60);
		 Set_Move(false);

		 if (m_bPlaySound == false)
		 {
			 m_pGameInstance->Play_Sound(L"EN70_EFFECT", L"Dash.wav", SOUND_EN70EFFECT1, 1.5f);
			 m_bPlaySound = true;
		 }

		 if (m_bPlaySound == true)
		 {
			 m_fDashTimeAcc += fTimeDelta;

			 if (m_fDashTimeAcc > m_fDashSoundTick)
			 {
				 m_bPlaySound = false;
				 m_fDashTimeAcc = 0.f;
			 }
		 }

		 _vector vMyPos = XMLoadFloat3(&Get_MyPosition());
		 _vector vEscapePos = XMLoadFloat3(&Get_EscapePosition());
		 vMyPos.m128_f32[3] = 1.f;
		 vEscapePos.m128_f32[3] = 1.f;
		 
		 m_pTransformCom->Go_Target_Navi(vEscapePos, fTimeDelta, m_pNavigationCom);

		 if (InRange(XMVectorGetX(XMVector3Length(vMyPos - vEscapePos)), 0.f, 2.f, "[]"))
		 {
				Reset_AnimSpeed();
			 m_pModelCom->Set_Loop(true);
			 Set_Move(true);
			 return BT_STATUS::Success;
		 }
		 else
		 {
			 if (false == m_pNavigationCom->isMove(vMyPos))
				 return BT_STATUS::Failure;

			return BT_STATUS::Running;
		 }
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




	 m_pBehaviorTree = Builder(EN70_BlackBoard)
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
					.leaf<FunctionNode>(Task_NormalSwingAttack1)
					.leaf<FunctionNode>(Task_NormalSwingAttack2)
					.leaf<FunctionNode>(Task_NormalSwingAttack3)
					.leaf<FunctionNode>(Task_NormalSwingAttack4)
					.leaf<FunctionNode>(Task_HorseDashReady)
					.leaf<FunctionNode>(Task_Escape)
					.leaf<FunctionNode>(Task_DashStop)
				.end()
			.end()
			.composite<Sequence>()
				.leaf<FunctionNode>(Control_InChaseRange)
				.composite<Selector>()
					.composite<Sequence>()
						.leaf<FunctionNode>(Control_DashRush)
						.leaf<FunctionNode>(Task_HorseDashReady)
						.leaf<FunctionNode>(Task_Dash)
						.leaf<FunctionNode>(Task_DashStop)
					.end()
					.composite<Sequence>()
						.leaf<FunctionNode>(Task_HorseDashReady)
						.leaf<FunctionNode>(Task_Escape)
						.leaf<FunctionNode>(Task_DashStop)
					.end()
				.end()
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

void CMonster_EN70::On_Trail()
{
	
	dynamic_cast<CMonsterPart_EN70_Weapon*>(Find_PartObject(TEXT("Part_Weapon")))->On_Trail();
	
}

void CMonster_EN70::Off_Trail()
{
	dynamic_cast<CMonsterPart_EN70_Weapon*>(Find_PartObject(TEXT("Part_Weapon")))->Off_Trail();
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
