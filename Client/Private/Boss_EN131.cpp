#include "stdafx.h"
#include "Boss_EN131.h"
#include "GameInstance.h"

#include "PartObject.h"
#include "Bone.h"
#include "Animation.h"
#include "BossPart_EN131_Weapon.h"
#include "Player.h"
#include "Camera_Target.h"
#include "Layer.h"
#include "Transform.h"


CBoss_EN131::CBoss_EN131(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice,pContext)
{
}

CBoss_EN131::CBoss_EN131(const CBoss_EN131& rhs)
	: CMonster(rhs)
{
}


CPartObject* CBoss_EN131::Find_PartObject(const wstring& strPartTag)
{
	auto iter = m_PartObjects.find(strPartTag);

	if (iter == m_PartObjects.end())
		return nullptr;

	return iter->second;
}

CCollider* CBoss_EN131::Get_HeadCollider(HEAD_COLLIDER eHeadType)
{
	CPartObject* pPartObject = nullptr;

	switch (eHeadType)
	{
	case Client::CBoss_EN131::LEFT:
		pPartObject = Find_PartObject(TEXT("Part_Weapon1"));
		break;
	case Client::CBoss_EN131::CENTER:
		pPartObject = Find_PartObject(TEXT("Part_Weapon2"));
		break;
	case Client::CBoss_EN131::RIGHT:
		pPartObject = Find_PartObject(TEXT("Part_Weapon3"));
		break;
	}

	return dynamic_cast<CCollider*>(pPartObject->Find_Component(TEXT("Com_Collider")));
}

CCollider* CBoss_EN131::Get_TailCollider(TAIL_COLLIDER eTailType)
{
	CPartObject* pPartObject = nullptr;

	switch (eTailType)
	{
	case Client::CBoss_EN131::STING:
		pPartObject = Find_PartObject(TEXT("Part_StingTail"));
		break;
	case Client::CBoss_EN131::SWING:
		pPartObject = Find_PartObject(TEXT("Part_SwingTail"));
		break;
	}
	
	return dynamic_cast<CCollider*>(pPartObject->Find_Component(TEXT("Com_Collider")));
}



HRESULT CBoss_EN131::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelID = eLevel;
	
	return S_OK;
}

HRESULT CBoss_EN131::Initialize(void* pArg)
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

	m_vCameraOffset = { 0.f, 10.f, -10.f };
	m_vJumpOffset = { 0.f, 24.f, -25.f };

	m_isBoss = true;

	Init_Status(2000.f, 30.f);

	if (FAILED(__super::Initialize_UI()))
		return E_FAIL;
	return S_OK;
}

void CBoss_EN131::Priority_Tick(_float fTimeDelta)
{

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Priority_Tick(fTimeDelta);
	}
}

void CBoss_EN131::Tick(_float fTimeDelta)
{
	_bool bTest = false;
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

		if(3.f >= pBlackBoard->getFloat("TargetLength"))
			bTest = true;
	}

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Tick(fTimeDelta);
	}

	_float3 vPos = {};

	m_pModelCom->Play_Animation(fTimeDelta, vPos);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	//if (false == m_bFindCell && true == m_bAppear)
	//{
	//	_int iCellIndex = m_pNavigationCom->Get_SelectRangeCellIndex(this);
	//
	//	m_pNavigationCom->Set_CurrentIndex(iCellIndex);
	//	m_bFindCell = true;
	//}
	//
	//if(m_eCurrentLevelID != LEVEL_TOOL && true == m_bAppear )
	//	m_pRigidBodyCom->Tick(fTimeDelta);

	_vector vRealPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vRealPos.m128_f32[0] += vPos.x;
	vRealPos.m128_f32[1] += vPos.y;
	vRealPos.m128_f32[2] += vPos.z;
	vRealPos.m128_f32[3] = 1.f;

	
	if (false == m_bTest)
	{
		m_pTransformCom->Add_LookPos(vPos);
	}

	if (m_eCurrentLevelID != LEVEL_TOOL)
	{
		//if (true == m_bAppear && false == bTest && true == m_pNavigationCom->isMove(vRealPos) && true == m_bMove)
		//	vPos.x *= 0.75f;
		//	vPos.z *= 0.75f;
		//	m_pTransformCom->Add_LookPos(vPos);

		//if (false == bTest && true == m_pNavigationCom->isMove(vRealPos) && true == m_bMove)
		//	vPos.x *= 0.75f;
		//vPos.z *= 0.75f;
		//m_pTransformCom->Add_LookPos(vPos);
	}

	
}

void CBoss_EN131::Late_Tick(_float fTimeDelta)
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

HRESULT CBoss_EN131::Render()
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
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR);

		m_pShaderCom->Begin(0); //! 셰이더에 던져주고 비긴 호출하는 걸 잊지말자

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG

	m_pColliderCom->Render();

#endif

	return S_OK;
}

HRESULT CBoss_EN131::Render_Shadow()
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

void CBoss_EN131::Write_Json(json& Out_Json)
{
	__super::Write_Json(Out_Json);
	
}

void CBoss_EN131::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);

}

void CBoss_EN131::Init_Desc()
{

}

void CBoss_EN131::On_Collision(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit)
{
	__super::On_Collision(pCollisionObject, LeftTag, RightTag, vCollisionPos, bType, bHit);

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

	if (RightTag == TEXT("Layer_Monster"))
	{
		_float magnitude = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vCollisionPos)));

		_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		_vector vForceDir = vRight * magnitude;

		_float3 vCalcPos;
		XMStoreFloat3(&vCalcPos, vForceDir);

		vCalcPos = { -vCalcPos.x, -vCalcPos.y, -vCalcPos.z };

		CRigidBody* pTargetBody = dynamic_cast<CRigidBody*>(pCollisionObject->Find_Component(TEXT("Com_RigidBody")));
		
		pTargetBody->Add_Force(vCalcPos, CRigidBody::FORCE_MODE::FORCE);

	}
}

void CBoss_EN131::On_CollisionEnter(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _float3& vCollisionPos, _bool bType, _bool bHit)
{
	if (bType == false)
	{
		Set_Move(false);
	}
	if (m_tStatus.eAttackType == tagStatusDesc::RUSH_ATTACK && RightTag == TEXT("Layer_Player"))
	{
		CRigidBody* pTargetBody = dynamic_cast<CRigidBody*>(pCollisionObject->Find_Component(TEXT("Com_RigidBody")));


		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 500.f;

		_float3 vForce;
		XMStoreFloat3(&vForce, vLook);

		pTargetBody->Add_Force(vForce, CRigidBody::FORCE_MODE::IMPULSE);

	}

	__super::On_CollisionEnter(pCollisionObject, LeftTag, RightTag, vCollisionPos, bType, bHit);
}

void CBoss_EN131::On_CollisionExit(CGameObject* pCollisionObject, wstring& LeftTag, wstring& RightTag, _bool bType, _bool bHit)
{
	if (bType == false)
	{
		Set_Move(true);
	}
	__super::On_CollisionExit(pCollisionObject, LeftTag, RightTag, bType, bHit);

	m_pRigidBodyCom->Clear_NetPower();
}

HRESULT CBoss_EN131::Ready_Components()
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
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Model_Boss_EN131"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBoundingBox_AABB::BOUNDING_AABB_DESC		BoundingDesc = {};

	BoundingDesc.vExtents = _float3(1.5f, 2.f, 3.f);
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

HRESULT CBoss_EN131::Ready_PartObjects()
{
	CPartObject::PART_DESC WeaponDesc = {};

	
	
	CBone* pSwordBone = m_pModelCom->Get_BonePtr("C_HEAD");

	if(nullptr == pSwordBone)
		return E_FAIL;

	WeaponDesc.m_pSocketBone = pSwordBone;
	WeaponDesc.m_pParentTransform = m_pTransformCom;
	WeaponDesc.m_strPartName = "C_HEAD";


	if(FAILED(Add_PartObject(TEXT("Prototype_PartObject_Boss_EN131_Weapon"), TEXT("Part_Weapon1"), &WeaponDesc)))
		return E_FAIL;

	pSwordBone = m_pModelCom->Get_BonePtr("L_HEAD");
	
	if (nullptr == pSwordBone)
		return E_FAIL;

	WeaponDesc.m_pSocketBone = pSwordBone;
	WeaponDesc.m_strPartName = "L_HEAD";

	if (FAILED(Add_PartObject(TEXT("Prototype_PartObject_Boss_EN131_Weapon"), TEXT("Part_Weapon2"), &WeaponDesc)))
		return E_FAIL;

	pSwordBone = m_pModelCom->Get_BonePtr("R_HEAD");

	if (nullptr == pSwordBone)
		return E_FAIL;

	WeaponDesc.m_pSocketBone = pSwordBone;
	WeaponDesc.m_strPartName = "R_HEAD";

	if (FAILED(Add_PartObject(TEXT("Prototype_PartObject_Boss_EN131_Weapon"), TEXT("Part_Weapon3"), &WeaponDesc)))
		return E_FAIL;

	//pSwordBone = m_pModelCom->Get_BonePtr("L_JAW");
	//
	//if (nullptr == pSwordBone)
	//	return E_FAIL;
	//
	//WeaponDesc.m_pSocketBone = pSwordBone;
	//WeaponDesc.m_strPartName = "L_FIRE";
	//
	//if (FAILED(Add_PartObject(TEXT("Prototype_PartObject_Boss_EN131_Weapon"), TEXT("Part_L_Fire"), &WeaponDesc)))
	//	return E_FAIL;
	//
	//pSwordBone = m_pModelCom->Get_BonePtr("C_JAW");
	//
	//if (nullptr == pSwordBone)
	//	return E_FAIL;
	//
	//WeaponDesc.m_pSocketBone = pSwordBone;
	//WeaponDesc.m_strPartName = "C_FIRE";
	//
	//if (FAILED(Add_PartObject(TEXT("Prototype_PartObject_Boss_EN131_Weapon"), TEXT("Part_C_Fire"), &WeaponDesc)))
	//	return E_FAIL;
	//
	//pSwordBone = m_pModelCom->Get_BonePtr("R_JAW");
	//
	//if (nullptr == pSwordBone)
	//	return E_FAIL;
	//
	//WeaponDesc.m_pSocketBone = pSwordBone;
	//WeaponDesc.m_strPartName = "R_FIRE";
	//
	//if (FAILED(Add_PartObject(TEXT("Prototype_PartObject_Boss_EN131_Weapon"), TEXT("Part_R_Fire"), &WeaponDesc)))
	//	return E_FAIL;


	pSwordBone = m_pModelCom->Get_BonePtr("TAIL7");

	if (nullptr == pSwordBone)
		return E_FAIL;

	WeaponDesc.m_pSocketBone = pSwordBone;
	WeaponDesc.m_strPartName = "TAIL7";

	if (FAILED(Add_PartObject(TEXT("Prototype_PartObject_Boss_EN131_Weapon"), TEXT("Part_StingTail"), &WeaponDesc)))
		return E_FAIL;

	pSwordBone = m_pModelCom->Get_BonePtr("TAIL3");

	if (nullptr == pSwordBone)
		return E_FAIL;

	WeaponDesc.m_pSocketBone = pSwordBone;
	WeaponDesc.m_strPartName = "TAIL3";

	if (FAILED(Add_PartObject(TEXT("Prototype_PartObject_Boss_EN131_SwingTail"), TEXT("Part_SwingTail"), &WeaponDesc)))
		return E_FAIL;

	return S_OK;
}



HRESULT CBoss_EN131::Ready_BehaviorTree_V2()
{
	m_pBehaviorTree = std::make_shared<BrainTree::BehaviorTree>();

	 BrainTree::Blackboard::Ptr EN131_BlackBoard = m_pBehaviorTree->getBlackboard();
	 	
	 EN131_BlackBoard->setString("Name", "EN00");
	 EN131_BlackBoard->setFloat("Max_HP", 2000.f);
	 //EN131_BlackBoard->setFloat("Current_HP", 2000.f);
	 EN131_BlackBoard->setFloat("Current_HP", 600.f);
	 EN131_BlackBoard->setFloat("Attack_Range", 10.f);
	 EN131_BlackBoard->setFloat("Keep_Range", 3.5f);
	 EN131_BlackBoard->setFloat("Detect_Range", 20.f);
	 EN131_BlackBoard->setFloat("Speed", m_pTransformCom->Get_Speed());
	 EN131_BlackBoard->setFloat("RotateSpeed", m_pTransformCom->Get_RotationSpeed());

	 //!EN00_BlackBoard->setFloat3("DashRushTargetPosition", vTargetPos);
	 //!EN00_BlackBoard->setFloat3("EscapePosition", _float3());
	 CGameObject* pTarget = nullptr;

	 pTarget = m_pGameInstance->Get_Player(m_eCurrentLevelID);

	 if (nullptr != pTarget)
	 {
		 EN131_BlackBoard->setTarget(pTarget);
	 }

	 _float3 vMyPos, vTargetPos;
	 
	 XMStoreFloat3(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	 XMStoreFloat3(&vTargetPos, pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION));

	 EN131_BlackBoard->setFloat3("MyPosition", vMyPos);
	 EN131_BlackBoard->setFloat3("TargetPosition", vTargetPos);
	 EN131_BlackBoard->setFloat3("BreathPosition", vTargetPos);
	 EN131_BlackBoard->setFloat("CurrentTrackPosition", 0.f);
	 EN131_BlackBoard->setFloat("TimeAcc", 0.f);

	 EN131_BlackBoard->setFloat("HowlingRange", 8.f);
	 //EN131_BlackBoard->setFloat3("TargetPostion", _float3(0.f, 0.f, 0.f));
	
	 
	 EN131_BlackBoard->setBool("Is_Sting", false);
	 EN131_BlackBoard->setBool("Is_Swing", false);
	 EN131_BlackBoard->setBool("Is_Howling", false);
	 EN131_BlackBoard->setBool("Is_Bite", false);
	 EN131_BlackBoard->setBool("Is_Hit", false); //! 피격중인가
	 EN131_BlackBoard->setBool("Is_Ground", true);
	 EN131_BlackBoard->setBool("Is_Appeal", false);
	 
	 EN131_BlackBoard->setBool("Is_Die", false);
	 EN131_BlackBoard->setBool("Is_Dead", false);
	
	 
	 //EN131_BlackBoard->setBool("Is_TargetPosition", false); //! 목표로 하는 지점이 있는가
	 EN131_BlackBoard->setBool("Is_Patrol", false); //! 순찰해야하는가?
	 EN131_BlackBoard->setBool("Is_OneTick", true);
	 EN131_BlackBoard->setInt("iRandomIndex", -1);
	 EN131_BlackBoard->setInt("iCurrentPhase", 1);

	 EN131_BlackBoard->setOwner(this);
	 EN131_BlackBoard->setModel(m_pModelCom);
	 EN131_BlackBoard->setShader(m_pShaderCom);
	 EN131_BlackBoard->setCollider(m_pColliderCom);
	 EN131_BlackBoard->setTransform(m_pTransformCom);
	 EN131_BlackBoard->setGameInstance(m_pGameInstance);
	 EN131_BlackBoard->setNavigation(m_pNavigationCom);
	 EN131_BlackBoard->setRigidBody(m_pRigidBodyCom);
	 
	 
//#EN131Tree
	FUNCTION_NODE Control_Appeal
		= FUNCTION_NODE_MAKE
	{
		if (true == m_bStart) 
		{
			m_pModelCom->Root_MotionEnd();
			m_pTransformCom->Set_SpeedPerSec(6.f);
			m_pTransformCom->Set_RotationPerSec(XMConvertToRadians(90.f));
		
			if (false == m_bJumpStart)
			{
				m_pModelCom->Set_Loop(true);
				Transition(1);
				m_pTransformCom->Go_Straight(fTimeDelta);
				m_fTimeAcc += fTimeDelta;

				if (m_fTimeAcc > 7.f)
				{
					Transition(0);
					m_bJumpStart = true;
					m_fTimeAcc = 0.f;

					CCamera_Target* pTargetCamera = dynamic_cast<CCamera_Target*>(m_pGameInstance->Find_Layer(m_eCurrentLevelID, TEXT("Layer_Camera"))->Get_ObjectList().back());
					_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
					vLook.m128_f32[3] = 0.f;
					pTargetCamera->Get_Transform()->Look_At_Dir(vLook);
				}
				return BT_STATUS::Running;
			}
		
			if (m_bJumpStart == true && true == m_bStartAppeal)
			{
				if (false == pBlackboard->getBool("Is_Appeal"))
				{
					CCamera_Target* pTargetCamera = dynamic_cast<CCamera_Target*>(m_pGameInstance->Find_Layer(m_eCurrentLevelID, TEXT("Layer_Camera"))->Get_ObjectList().back());
					pTargetCamera->Set_OffSet(m_vJumpOffset);
// 					_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
// 					vLook.m128_f32[3] = 0.f;
// 					pTargetCamera->Get_Transform()->Look_At_Dir(vLook);
					m_vPrevPointPos = pBlackboard->getFloat3("MyPosition");
 					m_pTransformCom->Set_Point_Gravity_Velocity(m_vAppealJumpPosition, m_vPrevPointPos, m_fMaxHeight, m_fMaxTime, &m_vVelocity, &m_fPointJumpGravity);
 					pBlackboard->setBool("Is_Appeal", true);
				}

				Transition(60);

				m_pModelCom->Set_Loop(false);
				_bool bInRange = false;

				_vector vTargetPos = XMLoadFloat3(&m_vAppealJumpPosition);
 				vTargetPos.m128_f32[3] = 1.f;

				_vector vMyPosition = XMLoadFloat3(&(pBlackboard->getFloat3("MyPosition")));
 				vMyPosition.m128_f32[3] = 1.f;

				if (true == InRange(XMVectorGetX(XMVector3Length(vMyPosition - vTargetPos)), 0.f, 1.f, "[]"))
 				{
					if (true == m_bOneTime)
					{
						m_fTimeAcc = 0.f;
						m_bOneTime = true;
					}

					
					if (true == Is_CurrentAnimEnd())
					{
						m_bStart = false;
						m_bAppear = true;
						return BT_STATUS::Success;
					}
					return BT_STATUS::Running;
				}
				else
				{
					m_fTimeAcc += fTimeDelta;
					m_pTransformCom->Point_Parabola(m_vAppealJumpPosition, m_vPrevPointPos, m_vVelocity, m_fTimeAcc, m_fPointJumpGravity);
					return BT_STATUS::Running;
				}
			}
 		}
 		else
 			return BT_STATUS::Failure;

		return BT_STATUS::Failure;
	};

	FUNCTION_NODE Task_ReturnPlayerCam
		= FUNCTION_NODE_MAKE
	{
		
		CCamera_Target* pTargetCamera = dynamic_cast<CCamera_Target*>(m_pGameInstance->Find_Layer(m_eCurrentLevelID, TEXT("Layer_Camera"))->Get_ObjectList().back());

		CCamera_Target::CUTSCENE_DESC Desc;

		Desc.fChaseSpeed = 130.f;
		Desc.fStopRange = 10.f;
		Desc.pChaseTarget = m_pGameInstance->Get_Player(m_eCurrentLevelID);
		XMStoreFloat3(&Desc.vStartPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		Desc.vStartPos.y += 5.f;
		XMStoreFloat3(&Desc.vChasePos, Desc.pChaseTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION));

		pTargetCamera->Set_OffSet(Desc.pChaseTarget->Get_Offset());
		pTargetCamera->Set_CutSceneDesc(&Desc);
		

		wstring strDepressTag = TEXT("PlayerState_Depress");
		dynamic_cast<CPlayer*>(Desc.pChaseTarget)->Transition(CStateMachine::STATETYPE::STATE_GROUND, strDepressTag);


		return BT_STATUS::Success;
	};
	
	 FUNCTION_NODE Control_IsPhase1
		 = FUNCTION_NODE_MAKE
	 {
		if (1334 >= pBlackboard->getFloat("Current_HP"))
		{
			pBlackboard->setInt("iCurrentPhase", 2);
			return BT_STATUS::Failure;
		}


		 if (1 == pBlackboard->getInt("iCurrentPhase"))
		 {
			 return BT_STATUS::Success;
		 }
		 else
			return BT_STATUS::Failure;
	 };

	 FUNCTION_NODE Control_IsPhase2
		 = FUNCTION_NODE_MAKE
	 {
		if (700 >= pBlackboard->getFloat("Current_HP"))
		{
			pBlackboard->setInt("iCurrentPhase", 3);
			return BT_STATUS::Failure;
		}

		 if (2 == pBlackboard->getInt("iCurrentPhase"))
		 {
			 return BT_STATUS::Success;
		 }
		 else
			return BT_STATUS::Failure;
	 };

	 FUNCTION_NODE Control_IsPhase3
		 = FUNCTION_NODE_MAKE
	 {
		 if (3 == pBlackboard->getInt("iCurrentPhase"))
		 {
			 return BT_STATUS::Success;
		 }
		 else
			return BT_STATUS::Failure;
	 };


	 FUNCTION_NODE Control_IsDead
	 = FUNCTION_NODE_MAKE
	 {
		 if (true == pBlackboard->getBool("Is_Dead"))
		 {
			m_pModelCom->Set_Loop(false);
			Transition(36);

			if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
			{
				m_pModelCom->Set_Loop(true);
				return BT_STATUS::Success;
			}
		 }
		 else
			return BT_STATUS::Failure;
	 };

	 FUNCTION_NODE Task_IsDead1
		 = FUNCTION_NODE_MAKE
	 {
		Transition(34);
		m_pModelCom->Set_Loop(false);

		 if(m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		 {
			 return BT_STATUS::Success;
		 }
		 else
			return BT_STATUS::Running;
	 };

	FUNCTION_NODE Task_IsDead2
		 = FUNCTION_NODE_MAKE
	 {
			Die(5.f);
			return BT_STATUS::Running;
	 };

	FUNCTION_NODE Task_IntroAnimation
		= FUNCTION_NODE_MAKE
	{	
		Transition(60);
		m_pModelCom->Set_Loop(false);


		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
		   
			return BT_STATUS::Success;
		}
		else
		   return BT_STATUS::Running;
	};

	FUNCTION_NODE Task_BackEvasion
		= FUNCTION_NODE_MAKE
	{
		   Transition(3);
		   m_pModelCom->Set_Loop(false);

		   _vector vTargetPos = XMLoadFloat3(&Get_TargetPosition());
		   vTargetPos.m128_f32[3] = 1.f;
		   m_pTransformCom->TurnToTarget(vTargetPos, fTimeDelta);

		   m_pModelCom->Root_MotionStart();
		   Set_Move(true);

		 if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		 {
			 m_pModelCom->Root_MotionEnd();
			 m_pModelCom->Set_Loop(true);
			 Set_Move(false);
			 return BT_STATUS::Success;
		 }
		 else
			return BT_STATUS::Running;
	};

	FUNCTION_NODE Task_LeftEvasion
		= FUNCTION_NODE_MAKE
	{
		 Transition(5);
		 m_pModelCom->Set_Loop(false);
		 m_pModelCom->Root_MotionStart();

		 _vector vTargetPos = XMLoadFloat3(&Get_TargetPosition());
		 vTargetPos.m128_f32[3] = 1.f;
		 m_pTransformCom->TurnToTarget(vTargetPos, fTimeDelta);
		 Set_Move(true);

		 if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		 {
			 m_pModelCom->Root_MotionEnd();
			 m_pModelCom->Set_Loop(true);
			 Set_Move(false);
			 return BT_STATUS::Success;
		 }
		 else
			return BT_STATUS::Running;
	};

	FUNCTION_NODE Task_RightEvasion
		= FUNCTION_NODE_MAKE
	{
			Transition(4);
		  m_pModelCom->Set_Loop(false);
		  m_pModelCom->Root_MotionStart();

		  _vector vTargetPos = XMLoadFloat3(&Get_TargetPosition());
		  vTargetPos.m128_f32[3] = 1.f;
		  m_pTransformCom->TurnToTarget(vTargetPos, fTimeDelta);

		  Set_Move(true);

		 if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		 {
			 m_pModelCom->Root_MotionEnd();
			 m_pModelCom->Set_Loop(true);
			 Set_Move(false);
			 
			 return BT_STATUS::Success;
		 }
		 else
			return BT_STATUS::Running;
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
				_int iRandomHitIndex = Random({ 8, 9});
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

		   if (InRange(XMVectorGetX(XMVector3Length(vPosition - vTargetPosition)), 0.f, pBlackboard->getFloat("Attack_Range"), "[]"))
		   {
				return BT_STATUS::Success;
		   }
		   else
		   {
				m_pModelCom->Set_Loop(true);
			   return BT_STATUS::Failure;
		   }
	 };

	 FUNCTION_NODE Control_RandomTry
		= FUNCTION_NODE_MAKE
	 {
		_bool bRandomTry = Random({true, false, true, false, false, true, true, false, true, false});

		_vector vTargetPosition = XMLoadFloat3(&pBlackboard->getFloat3("TargetPosition"));
		vTargetPosition.m128_f32[3] = 1.f;

		m_pTransformCom->Look_At(vTargetPosition);


		if (true == bRandomTry)
			return BT_STATUS::Success;

		else
			return BT_STATUS::Failure;
	 };


	 FUNCTION_NODE Task_TailStingAttack1
		 = FUNCTION_NODE_MAKE
	 {
		Transition(37);
		m_tStatus.eAttackType = tagStatusDesc::DOWN_ATTACK;
		if (45.f < pBlackboard->getFloat("CurrentTrackPosition") && false == pBlackboard->getBool("Is_Sting"))
		{
			Get_TailCollider(CBoss_EN131::STING)->OnCollider();
			pBlackboard->setBool("Is_Sting", true);
		}
		 m_pModelCom->Set_Loop(false);

		 if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		 {
			 Get_TailCollider(CBoss_EN131::STING)->OffCollider();
			 pBlackboard->setBool("Is_Sting", false);

			 m_pModelCom->Set_Loop(true);
			 m_pModelCom->Get_CurrentAnimation()->Reset_Animation();
			 return BT_STATUS::Success;
		 }
		else
			return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_TailStingAttack2
		 = FUNCTION_NODE_MAKE
	 {
		Transition(38);
		m_pModelCom->Set_Loop(false);

		if (30.f < pBlackboard->getFloat("CurrentTrackPosition") && false == pBlackboard->getBool("Is_Sting"))
		{
			Get_TailCollider(CBoss_EN131::STING)->OnCollider();
			pBlackboard->setBool("Is_Sting", true);
		}

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			Get_TailCollider(CBoss_EN131::STING)->OffCollider();
			pBlackboard->setBool("Is_Sting", false);
			 m_pModelCom->Set_Loop(true);
			 m_pModelCom->Get_CurrentAnimation()->Reset_Animation();
			 return BT_STATUS::Success;
		}
		 else
			 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_TailStingAttack3
		 = FUNCTION_NODE_MAKE
	 {
		Transition(39);
		m_pModelCom->Set_Loop(false);

		if (51.f < pBlackboard->getFloat("CurrentTrackPosition") && false == pBlackboard->getBool("Is_Sting"))
		{
			Get_TailCollider(CBoss_EN131::STING)->OnCollider();
			pBlackboard->setBool("Is_Sting", true);
		}

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			Get_TailCollider(CBoss_EN131::STING)->OffCollider();
			pBlackboard->setBool("Is_Sting", false);
			m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Success;
		}
		else
			 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_TailStingAttack4
		 = FUNCTION_NODE_MAKE
	 {
		m_tStatus.eAttackType = tagStatusDesc::NORMAL_ATTACK;
		Transition(40);
		m_pModelCom->Set_Loop(false);
		
		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{

			m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Success;
		}
		 else
			 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_TailSwingAttack
		 = FUNCTION_NODE_MAKE
	 {
		m_tStatus.eAttackType = tagStatusDesc::RUSH_ATTACK;
		Transition(52);
		m_pModelCom->Set_Loop(false);

		if (116.f < pBlackboard->getFloat("CurrentTrackPosition") && false == pBlackboard->getBool("Is_Swing"))
		{
			
			Get_TailCollider(CBoss_EN131::SWING)->OnCollider();
			pBlackboard->setBool("Is_Swing", true);
		}

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			Get_TailCollider(CBoss_EN131::SWING)->OffCollider();
			pBlackboard->setBool("Is_Swing", false);
			m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Success;
		}
		 else
			 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_RoarMonsterSpawn
		 = FUNCTION_NODE_MAKE
	 {
		Transition(42);
		m_pModelCom->Set_Loop(false);

		if (112.f < pBlackboard->getFloat("CurrentTrackPosition") && false == pBlackboard->getBool("Is_Howling"))
		{
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Player(m_eCurrentLevelID));
			wstring strSturnTag = TEXT("PlayerState_Sturn");
			pPlayer->Transition(CStateMachine::STATETYPE::STATE_GROUND, strSturnTag);
			pBlackboard->setBool("Is_Howling", true);
		}

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			
			pBlackboard->setBool("Is_Howling", false);
			m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Success;
		}
		else
			 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_All_DirectionBite1
		 = FUNCTION_NODE_MAKE
	 {
		Transition(55);
		m_pModelCom->Set_Loop(false);

		m_tStatus.eAttackType = tagStatusDesc::DOWN_ATTACK;

		if (28.f < pBlackboard->getFloat("CurrentTrackPosition") && false == pBlackboard->getBool("Is_Bite"))
		{
			Get_HeadCollider(RIGHT)->OnCollider();
			pBlackboard->setBool("Is_Bite", true);
		}

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			Get_HeadCollider(RIGHT)->OffCollider();
			Get_HeadCollider(CENTER)->OnCollider();
			m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Success;
		}
		else
			 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_All_DirectionBite2
		 = FUNCTION_NODE_MAKE
	 {
		Transition(56);
		m_pModelCom->Set_Loop(false);

		if (4.f < pBlackboard->getFloat("CurrentTrackPosition") && true == pBlackboard->getBool("Is_Bite"))
		{
			Get_HeadCollider(CENTER)->OffCollider();
			pBlackboard->setBool("Is_Bite", false);
		}

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			pBlackboard->setBool("Is_Bite", false);
			m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Success;
		}
		else
			 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_All_DirectionBite3
		 = FUNCTION_NODE_MAKE
	 {
		Transition(57);
		m_pModelCom->Set_Loop(false);

		if (18.f < pBlackboard->getFloat("CurrentTrackPosition") && false == pBlackboard->getBool("Is_Bite"))
		{
			Get_HeadCollider(LEFT)->OnCollider();
			pBlackboard->setBool("Is_Bite", true);
		}

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			Get_HeadCollider(LEFT)->OffCollider();
			pBlackboard->setBool("Is_Bite", false);
			m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Success;
		}
		else
			 return BT_STATUS::Running;
	 };

	
	 FUNCTION_NODE Task_FullBite1
		 = FUNCTION_NODE_MAKE
	 {

		m_tStatus.eAttackType = tagStatusDesc::DOWN_ATTACK;

		Transition(53);
		m_pModelCom->Set_Loop(false);

		if (115.f < pBlackboard->getFloat("CurrentTrackPosition") && false == pBlackboard->getBool("Is_Bite"))
		{
			Get_HeadCollider(LEFT)->OnCollider();
			Get_HeadCollider(CENTER)->OnCollider();
			Get_HeadCollider(RIGHT)->OnCollider();
			pBlackboard->setBool("Is_Bite", true);
		}

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			Get_HeadCollider(LEFT)->OffCollider();
			Get_HeadCollider(CENTER)->OffCollider();
			Get_HeadCollider(RIGHT)->OffCollider();
			pBlackboard->setBool("Is_Bite", false);
			m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Success;
		}
		else
			 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_FullBite2
		 = FUNCTION_NODE_MAKE
	 {
		Transition(54);
		m_pModelCom->Set_Loop(false);

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Success;
		}
		else
			 return BT_STATUS::Running;
	 };


	 FUNCTION_NODE Task_OrderFullBite1
		 = FUNCTION_NODE_MAKE
	 {
		m_tStatus.eAttackType = tagStatusDesc::DOWN_ATTACK;

		if (71.f < pBlackboard->getFloat("CurrentTrackPosition") && false == pBlackboard->getBool("Is_Bite"))
		{
			 Get_HeadCollider(LEFT)->OnCollider();
			 Get_HeadCollider(CENTER)->OnCollider();
			 Get_HeadCollider(RIGHT)->OnCollider();
			 pBlackboard->setBool("Is_Bite", true);
		}

		Transition(43);
		m_pModelCom->Set_Loop(false);
		
		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			Get_HeadCollider(LEFT)->OffCollider();
			Get_HeadCollider(CENTER)->OffCollider();
			Get_HeadCollider(RIGHT)->OffCollider();
			pBlackboard->setBool("Is_Bite", false);

			m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Success;
		}
		else
			 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_OrderFullBite2
		 = FUNCTION_NODE_MAKE
	 {
		Transition(44);
		m_pModelCom->Set_Loop(false);

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Success;
		}
		else
			 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_Breath1
		 = FUNCTION_NODE_MAKE
	 {
		m_tStatus.eAttackType = tagStatusDesc::NORMAL_ATTACK;

		Transition(45);
		m_pModelCom->Set_Loop(false);

		if (true == pBlackboard->getBool("Is_OneTick"))
		{
			pBlackboard->setFloat3("BreathPostion", Get_TargetPosition());
			pBlackboard->setBool("Is_OneTick", false);
		}
		

		if (128.f < pBlackboard->getFloat("CurrentTrackPosition") && false == pBlackboard->getBool("Is_Breath"))
		{
			pBlackboard->setBool("Is_Breath", true);


			CBossPart_EN131_Weapon* pLeftHead = dynamic_cast<CBossPart_EN131_Weapon*>(Find_PartObject(TEXT("Part_Weapon1")));
			CBossPart_EN131_Weapon* pCenterHead = dynamic_cast<CBossPart_EN131_Weapon*>(Find_PartObject(TEXT("Part_Weapon2")));
			CBossPart_EN131_Weapon* pRightHead = dynamic_cast<CBossPart_EN131_Weapon*>(Find_PartObject(TEXT("Part_Weapon3")));

			//CBossPart_EN131_Weapon* pLeftFire = dynamic_cast<CBossPart_EN131_Weapon*>(Find_PartObject(TEXT("Part_L_Fire")));
			//CBossPart_EN131_Weapon* pCenterFire = dynamic_cast<CBossPart_EN131_Weapon*>(Find_PartObject(TEXT("Part_C_Fire")));
			//CBossPart_EN131_Weapon* pRightFire = dynamic_cast<CBossPart_EN131_Weapon*>(Find_PartObject(TEXT("Part_R_Fire")));
			
			
			_float3 vBreathPos = pBlackboard->getFloat3("BreathPostion");
			
			_float3 vMyPos = pBlackboard->getFloat3("MyPosition");

			pLeftHead->CreateBreath(XMLoadFloat3(&vBreathPos));
			pCenterHead->CreateBreath(XMLoadFloat3(&vBreathPos));
			pRightHead->CreateBreath(XMLoadFloat3(&vBreathPos));
			//pLeftFire->CreateBreath(XMLoadFloat3(&vBreathPos));
			//pCenterFire->CreateBreath(XMLoadFloat3(&vBreathPos));
			//pRightFire->CreateBreath(XMLoadFloat3(&vBreathPos));
		}

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			pBlackboard->setBool("Is_OneTick", true);
			m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Success;
		}
		else
			 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_Breath2
		 = FUNCTION_NODE_MAKE
	 {
		Transition(46);
		m_pModelCom->Set_Loop(false);

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Success;
		}
		else
			 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_Breath3
		 = FUNCTION_NODE_MAKE
	 {
		Transition(47);
		m_pModelCom->Set_Loop(false);

		if (63.f < pBlackboard->getFloat("CurrentTrackPosition") && true == pBlackboard->getBool("Is_Breath"))
		{
			pBlackboard->setBool("Is_Breath", false);
		}

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Success;
		}
		else
			 return BT_STATUS::Running;
	 };

	 FUNCTION_NODE Task_BreathFullAnimation
		 = FUNCTION_NODE_MAKE
	 {
		Transition(51);
		m_pModelCom->Set_Loop(false);

		if (m_pModelCom->Get_CurrentAnimation()->Get_Finished())
		{
			m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Success;
		}
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
		  pBlackboard->setFloat("TargetLength", XMVectorGetX(XMVector3Length(vPosition - vTargetPosition)));
		   m_pTransformCom->TurnToTarget(vTargetPosition, fTimeDelta);
		 //_vector vDir = XMLoadFloat3(&pBlackboard->getFloat3("TargetPosition")) - XMLoadFloat3(&pBlackboard->getFloat3("MyPosition"));
		 //_float fDistance = XMVectorGetX(XMVector3Length(vDir));

		 if (InRange(XMVectorGetX(XMVector3Length(vPosition - vTargetPosition)), 0.f, pBlackboard->getFloat("Detect_Range"), "[]"))
		 {
			 m_pModelCom->Root_MotionStart();
				m_pModelCom->Set_Loop(false);
				m_pTransformCom->Look_At(vTargetPosition);
				Transition(2);
				Set_Move(true);
			

			if (true == m_pModelCom->Get_CurrentAnimation()->Get_Finished())
			{
				m_pModelCom->Root_MotionEnd();
				m_pModelCom->Set_Loop(true);
				Set_Move(false);
				return BT_STATUS::Success;
			}
			else
				return BT_STATUS::Running;
		 }
		 else
		 {
			 Set_Move(true);
			 m_pModelCom->Set_Loop(true);
			 return BT_STATUS::Failure;
		 }
			 
	 };

// 	 FUNCTION_NODE Task_InChaseRange
// 		 = FUNCTION_NODE_MAKE
// 	 {
// 
// 		
// 
// 		 if (true == pBlackboard->getBool("Is_Hit") || 0 >= pBlackboard->getFloat("Current_HP"))
// 			 return BT_STATUS::Success;
// 
// 		_vector vTargetPosition = XMLoadFloat3(&pBlackboard->getFloat3("TargetPosition"));
// 		vTargetPosition.m128_f32[3] = 1.f;
// 
// 		m_pTransformCom->Go_Target_Navi(vTargetPosition, fTimeDelta, pBlackboard->GetNavigation());
// 			
// 		return BT_STATUS::Success;
// 	 };

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




//#EN131Tree
	 m_pBehaviorTree = Builder(EN131_BlackBoard)
		.composite<Selector>()
			.composite<Sequence>()
				.leaf<FunctionNode>(Control_Appeal)
				.leaf<FunctionNode>(Task_ReturnPlayerCam)
			.end()
			.composite<Sequence>()	
				.leaf<FunctionNode>(Control_IsDead)
				.leaf<FunctionNode>(Task_IsDead1)
				.leaf<FunctionNode>(Task_IsDead2)
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
							.leaf<FunctionNode>(Control_IsPhase1)
								.composite<Selector>()
									.composite<Sequence>()
										.leaf<FunctionNode>(Control_RandomTry)
										.leaf<FunctionNode>(Task_TailStingAttack1)
										.leaf<FunctionNode>(Task_TailStingAttack2)
										.leaf<FunctionNode>(Task_TailStingAttack3)
										.leaf<FunctionNode>(Task_TailStingAttack4)
										.leaf<FunctionNode>(Control_RandomTry)
										.leaf<FunctionNode>(Task_LeftEvasion)
									.end()
									.composite<Sequence>()
										.leaf<FunctionNode>(Task_FullBite1)
										.leaf<FunctionNode>(Task_FullBite2)
										.leaf<FunctionNode>(Control_RandomTry)
										.leaf<FunctionNode>(Task_LeftEvasion)
									.end()
									.leaf<FunctionNode>(Task_RightEvasion)
								.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Control_IsPhase2)
								.composite<Selector>()
									.composite<Sequence>()
										.leaf<FunctionNode>(Task_OrderFullBite1)
										.leaf<FunctionNode>(Task_OrderFullBite2)
										.leaf<FunctionNode>(Control_RandomTry)
										.leaf<FunctionNode>(Task_BackEvasion)
										.leaf<FunctionNode>(Task_Breath1)
										.leaf<FunctionNode>(Task_Breath2)
										.leaf<FunctionNode>(Task_Breath3)
									.end()
									.composite<Sequence>()
										.leaf<FunctionNode>(Control_RandomTry)
										.leaf<FunctionNode>(Task_TailStingAttack1)
										.leaf<FunctionNode>(Task_TailStingAttack2)
										.leaf<FunctionNode>(Task_TailStingAttack3)
										.leaf<FunctionNode>(Task_TailStingAttack4)
										.leaf<FunctionNode>(Control_RandomTry)
										.leaf<FunctionNode>(Task_LeftEvasion)
									.end()

									.composite<Sequence>()
										.leaf<FunctionNode>(Control_RandomTry)
										.leaf<FunctionNode>(Task_RightEvasion)
									.end()
									.leaf<FunctionNode>(Task_LeftEvasion)
								.end()
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Control_IsPhase3)
								.composite<Selector>()
									.composite<Sequence>()
										//.leaf<FunctionNode>(Control_RandomTry)
										//.leaf<FunctionNode>(Task_TailSwingAttack)
										//.leaf<FunctionNode>(Task_BackEvasion)
										//.leaf<FunctionNode>(Control_RandomTry)
										.leaf<FunctionNode>(Task_Breath1)
										.leaf<FunctionNode>(Task_Breath2)
										.leaf<FunctionNode>(Task_Breath3)
									.end()
									.composite<Sequence>()
										.leaf<FunctionNode>(Task_All_DirectionBite1)
										.leaf<FunctionNode>(Task_All_DirectionBite2)
										.leaf<FunctionNode>(Task_All_DirectionBite3)
										.leaf<FunctionNode>(Control_RandomTry)
											.leaf<FunctionNode>(Task_BackEvasion)
										.leaf<FunctionNode>(Control_RandomTry)
											.leaf<FunctionNode>(Task_Breath1)
											.leaf<FunctionNode>(Task_Breath2)
											.leaf<FunctionNode>(Task_Breath3)
									.end()
									.composite<Sequence>()
										.leaf<FunctionNode>(Control_RandomTry)
										.leaf<FunctionNode>(Task_RoarMonsterSpawn)
										.leaf<FunctionNode>(Task_BackEvasion)
									.end()
									.composite<Sequence>()
										.leaf<FunctionNode>(Control_RandomTry)
										.leaf<FunctionNode>(Task_LeftEvasion)
									.end()
									.leaf<FunctionNode>(Task_RightEvasion)
								.end()
						.end()
					.end()
				.end()
			.end()
			.leaf<FunctionNode>(Control_InChaseRange)
			.leaf<FunctionNode>(Task_Idle)
		.end()
	.build();
	
	
	return S_OK;
}



HRESULT CBoss_EN131::Add_PartObject(const wstring& strPrototypeTag, const wstring& strPartTag, void* pArg)
{
	if (nullptr != Find_PartObject(strPrototypeTag))
		return E_FAIL;

	CGameObject* pPartObject = m_pGameInstance->Get_CloneObject(strPrototypeTag, pArg);

	if (nullptr == pPartObject)
		return E_FAIL;

	m_PartObjects.emplace(strPartTag, dynamic_cast<CPartObject*>(pPartObject));

	return S_OK;
}

HRESULT CBoss_EN131::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	
	return S_OK;
}


CBoss_EN131* CBoss_EN131::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CBoss_EN131* pInstance = new CBoss_EN131(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CBoss_EN131");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBoss_EN131::Clone(void* pArg)
{
	CBoss_EN131* pInstance = new CBoss_EN131(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_EN131");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_EN131::Free()
{
	__super::Free();

	for (auto& Pair : m_PartObjects)
		Safe_Release(Pair.second);

	m_PartObjects.clear();
}
