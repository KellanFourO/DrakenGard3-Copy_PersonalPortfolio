#include "stdafx.h"
#include "Monster_EN00.h"
#include "GameInstance.h"
#include "EN00State_Idle.h"
#include "EN00State_Chase.h"
#include "EN00State_Attack.h"

#include "EN00States.h"

#include "PartObject.h"
#include "Bone.h"

CMonster_EN00::CMonster_EN00(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice,pContext)
{
}

CMonster_EN00::CMonster_EN00(const CMonster_EN00& rhs)
	: CMonster(rhs)
{
}

CPartObject* CMonster_EN00::Find_PartObject(const wstring& strPartTag)
{
	auto iter = m_PartObjects.find(strPartTag);

	if (iter == m_PartObjects.end())
		return nullptr;

	return iter->second;
}

HRESULT CMonster_EN00::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelID = eLevel;
	
	return S_OK;
}

HRESULT CMonster_EN00::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;


	if (m_eCurrentLevelID != LEVEL_TOOL)
	{
		if (FAILED(Ready_BehaviorTree()))
			return E_FAIL;
	}

	m_pModelCom->Root_MotionStart();

	//m_pModelCom->Set_Animation(rand() % 20);

	// 클라 테스트용 m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(rand() % 20, 3.f, rand() % 20, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(rand() % 20, 0.f, rand() % 20, 1.f));

	return S_OK;
}

void CMonster_EN00::Priority_Tick(_float fTimeDelta)
{

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Priority_Tick(fTimeDelta);
	}
}

void CMonster_EN00::Tick(_float fTimeDelta)
{
	if (m_eCurrentLevelID != LEVEL_TOOL)
	{
		m_BehaviorTree->getBlackboard()->setTimeDelta(fTimeDelta);
		BrainTree::Node::Status TreeStatus = m_BehaviorTree->update();

		Debug_KeyInput();
	}

	

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Tick(fTimeDelta);
	}


	_float3 vPos = {};

	m_pModelCom->Play_Animation(fTimeDelta, vPos);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	m_pTransformCom->Add_LookPos(vPos);
}

void CMonster_EN00::Late_Tick(_float fTimeDelta)
{

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Late_Tick(fTimeDelta);
	}

	if (m_eCurrentLevelID != LEVEL_TOOL)
	{
		CCollider* pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_PartComponent(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), TEXT("Part_Weapon")));

		if (m_pColliderCom->Collision(pTargetCollider))
		{
			int i = 0;
		}
	}

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CMonster_EN00::Render()
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

void CMonster_EN00::Write_Json(json& Out_Json)
{
	__super::Write_Json(Out_Json);
	
	//Out_Json["MonsterDesc"]["MonsterType"] =		m_tLinkStateDesc.eMonType;
	//Out_Json["MonsterDesc"]["IdleType_Monster"] =	m_tLinkStateDesc.eNorMonIdleType;
	//Out_Json["MonsterDesc"]["IdleType_Boss"] =		m_tLinkStateDesc.eBossStartType;
	//Out_Json["MonsterDesc"]["Patrol"] =				m_tLinkStateDesc.bPatrol;
	//Out_Json["MonsterDesc"]["SectionIndex"] =		m_tLinkStateDesc.iSectionIndex;

	//auto iter = Out_Json["Component"].find("Model");
	//Out_Json["Component"].erase(iter);
}

void CMonster_EN00::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);

	
	//m_tLinkStateDesc.Reset();
	//
	//m_tLinkStateDesc.eMonType = In_Json["MonsterDesc"]["MonsterType"];
	//m_tLinkStateDesc.eNorMonIdleType = In_Json["MonsterDesc"]["IdleType_Monster"];
	//m_tLinkStateDesc.eBossStartType = In_Json["MonsterDesc"]["IdleType_Boss"];
	//
	//if (In_Json["MonsterDesc"].end() != In_Json["MonsterDesc"].find("Patrol"))
	//	m_tLinkStateDesc.bPatrol = In_Json["MonsterDesc"]["Patrol"];
	//
	//if (In_Json["MonsterDesc"].end() != In_Json["MonsterDesc"].find("SectionIndex"))
	//	m_tLinkStateDesc.iSectionIndex = In_Json["MonsterDesc"]["SectionIndex"];
	//
	//XMStoreFloat4(&m_tLinkStateDesc.m_fStartPositon, m_pTransformCom.lock()->Get_State(CTransform::STATE_TRANSLATION));
	//
	//GET_SINGLE(CGameManager)->Registration_Section(m_tLinkStateDesc.iSectionIndex, Weak_Cast<CGameObject>(m_this));
	//
	//Init_Desc();
}

void CMonster_EN00::Init_Desc()
{
	//m_pStatus.lock()->Init_Status(&m_tLinkStateDesc);
}

HRESULT CMonster_EN00::Ready_Components()
{
	CNavigation::NAVI_DESC		NaviDesc = {};
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	/* For.Com_Shader */ //#모델셰이더는_Prototype_Component_Shader_Model
	//Prototype_Component_Shader_AnimModel
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_AnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Model_Monster_EN00"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBoundingBox_AABB::BOUNDING_AABB_DESC		BoundingDesc = {};

	BoundingDesc.vExtents = _float3(0.4f, 0.7f, 0.4f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CMonster_EN00::Ready_PartObjects()
{
	CPartObject::PART_DESC WeaponDesc = {};

	
	
	CBone* pSwordBone = m_pModelCom->Get_BonePtr("M_WP1");

	if(nullptr == pSwordBone)
		return E_FAIL;

	WeaponDesc.m_pSocketBone = pSwordBone;
	WeaponDesc.m_pParentTransform = m_pTransformCom;


	if(FAILED(Add_PartObject(TEXT("Prototype_PartObject_Monster_EN00_Weapon"), TEXT("Part_Weapon"), &WeaponDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_EN00::Ready_BehaviorTree()
{
	m_BehaviorTree = std::make_shared<BrainTree::BehaviorTree>();
	
	BrainTree::Blackboard::Ptr EN00_BlackBoard = m_BehaviorTree->getBlackboard();
	
	EN00_BlackBoard->setString("Name", "EN00");
	EN00_BlackBoard->setFloat("Max_HP", 100.f);
	EN00_BlackBoard->setFloat("Current_HP", 100.f);
	EN00_BlackBoard->setFloat("Attack_Range", 3.f);
	EN00_BlackBoard->setFloat("Keep_Range", 3.5f);
	EN00_BlackBoard->setFloat("Detect_Range", 10.f);
	EN00_BlackBoard->setFloat("Speed", m_pTransformCom->Get_Speed());
	EN00_BlackBoard->setFloat("RotateSpeed", m_pTransformCom->Get_RotationSpeed());

	EN00_BlackBoard->setFloat3("TargetPostion", _float3(0.f, 0.f, 0.f));

	EN00_BlackBoard->setBool("Is_Hit", false); //! 피격중인가
	EN00_BlackBoard->setBool("Is_Ground", true);
	EN00_BlackBoard->setBool("Is_Dead", false);
	EN00_BlackBoard->setBool("Is_TargetPosition", false); //! 목표로 하는 지점이 있는가
	EN00_BlackBoard->setBool("Is_Patrol", false); //! 순찰해야하는가?


	EN00_BlackBoard->setOwner(this);
	EN00_BlackBoard->setModel(m_pModelCom);
	EN00_BlackBoard->setShader(m_pShaderCom);
	EN00_BlackBoard->setCollider(m_pColliderCom);
	EN00_BlackBoard->setTransform(m_pTransformCom);
	EN00_BlackBoard->setGameInstance(m_pGameInstance);
	EN00_BlackBoard->setNavigation(m_pNavigationCom);

	


	CGameObject* pTarget = nullptr;

	
	pTarget = m_pGameInstance->Get_Player(m_eCurrentLevelID);

	if (nullptr != pTarget)
	{
		EN00_BlackBoard->setTarget(pTarget);
	}


	
	/* BrainTree 최상위 생성 */
	BrainTree::Builder EN00_Builder(EN00_BlackBoard);
	
	//// 셀렉터를 루트로 설정
	auto selectorBuilder = EN00_Builder.composite<BrainTree::Selector>();  //! 

	//TODO  첫 번째 시퀀스 추가 
	//!	( 죽었는가, 죽는 액션 및 소멸 처리 )
	selectorBuilder.composite<BrainTree::Sequence>() 
		.leaf<CEN00_Control_Dead>()  // 첫 번째 리프 노드 추가
		.leaf<CEN00_Task_Dead>() // 두 번째 리프 노드 추가
		.end() 

		.composite<BrainTree::Sequence>() 
		.leaf<CEN00_Control_Ground>()
		.composite<BrainTree::Selector>()  //! 셀렉터 4
		.composite<BrainTree::Sequence>()
		.leaf<CEN00_Control_Hit>()
		.leaf<CEN00_Task_Hit>()
		.end()
		
		.composite<BrainTree::Sequence>()  //! 시퀀스 5
		.leaf< CEN00_Control_AttackRange>()
		.composite<BrainTree::Selector>() //! 셀렉터 6

		.composite<BrainTree::Sequence>() //! 시퀀스 7
		.leaf< CEN00_Control_KeepEye>()
		.leaf< CEN00_Task_KeepEye>()
		.end()
		

		.composite<BrainTree::Selector>()//! 셀렉터 8

		.composite<BrainTree::Sequence>()//! 시퀀스 9
		.leaf<CEN00_Control_NormalAttack>()
		.leaf<CEN00_Task_NormalAttack>()
		.end()

		.composite<BrainTree::Sequence>() //! 시퀀스 10
		.leaf<CEN00_Control_UpperAttack>()
		.leaf<CEN00_Task_UpperAttack>()
		.end()

		.composite<BrainTree::Sequence>()//! 시퀀스 11
		.leaf<CEN00_Control_ChargeAttack>()
		.leaf<CEN00_Task_ChargeAttack>()
		.end()
		.end()
		.end()
		.end()
		

		.composite<BrainTree::Sequence>() //! 시퀀스 12
		.leaf<CEN00_Control_DetectRange>()
		.leaf<CEN00_Task_DetectRange>()
		.end()

		.composite<BrainTree::Sequence>() //! 시퀀스 13
		.leaf<CEN00_Control_PlayerDead>()
		.composite<BrainTree::Selector>() //! 셀렉터 14

		.composite<BrainTree::Sequence>() //! 시퀀스 15
		.leaf<CEN00_Control_Arrival>()
		.leaf<CEN00_Task_Idle>()
		.end()

		.composite<BrainTree::Sequence>()//! 시퀀스 16
		.leaf<CEN00_Control_SetTargetPosition>()
		.leaf<CEN00_Task_MoveToTarget>()
		.end()
		.end()
		.end()
		.end()
		.end()
		.end();

	BrainTree::Node::Ptr RootNode = EN00_Builder.build();

	m_BehaviorTree->setRoot(RootNode);

	
	return S_OK;
}


//HRESULT CMonster_EN00::Ready_States()
//{
//	if (FAILED(m_pStateCom->Add_State(TEXT("EN00State_Idle"), CEN00State_Idle::Create(this))))
//		return E_FAIL;
//	
//	if(FAILED(m_pStateCom->Set_InitState(TEXT("EN00State_Idle"))))
//		return E_FAIL;
//
//	return S_OK;
//}

HRESULT CMonster_EN00::Add_PartObject(const wstring& strPrototypeTag, const wstring& strPartTag, void* pArg)
{
	if (nullptr != Find_PartObject(strPrototypeTag))
		return E_FAIL;

	CGameObject* pPartObject = m_pGameInstance->Get_CloneObject(strPrototypeTag, pArg);

	if (nullptr == pPartObject)
		return E_FAIL;

	m_PartObjects.emplace(strPartTag, dynamic_cast<CPartObject*>(pPartObject));

	return S_OK;
}

HRESULT CMonster_EN00::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	
	return S_OK;
}

void CMonster_EN00::Debug_KeyInput()
{
	BrainTree::Blackboard::Ptr pBlackBoard = m_BehaviorTree->getBlackboard();

	if (m_pGameInstance->Key_Down(DIK_NUMPAD7))
	{
		pBlackBoard->setBool("Is_Dead", true);
	}

	if (m_pGameInstance->Key_Down(DIK_NUMPAD8))
	{
		pBlackBoard->setBool("Is_Dead", false);
	}

	if (m_pGameInstance->Key_Down(DIK_NUMPAD4))
	{
		pBlackBoard->setBool("Is_Hit", true);
	}

	if (m_pGameInstance->Key_Down(DIK_NUMPAD5))
	{
		pBlackBoard->setBool("Is_Hit", false);
	}

	if (m_pGameInstance->Key_Down(DIK_NUMPAD1))
	{
		pBlackBoard->setBool("Is_TargetPosition", true);
		_float3 vTargetPos;
		XMStoreFloat3(&vTargetPos, pBlackBoard->GetTarget()->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		pBlackBoard->setFloat3("TargetPostion", vTargetPos);
	}

	if (m_pGameInstance->Key_Down(DIK_NUMPAD2))
	{
		pBlackBoard->setBool("Is_TargetPosition", false);

		pBlackBoard->setFloat3("TargetPostion", _float3(0.f, 0.f, 0.f));
	}
}


CMonster_EN00* CMonster_EN00::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CMonster_EN00* pInstance = new CMonster_EN00(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CMonster_EN00");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMonster_EN00::Clone(void* pArg)
{
	CMonster_EN00* pInstance = new CMonster_EN00(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster_EN00");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonster_EN00::Free()
{
	__super::Free();

	for (auto& Pair : m_PartObjects)
		Safe_Release(Pair.second);

		m_PartObjects.clear();
}
