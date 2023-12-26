#include "stdafx.h"
#include "Monster_EN00.h"
#include "GameInstance.h"
#include "EN00State_Idle.h"
#include "PartObject.h"
#include "Bone.h"

CMonster_EN00::CMonster_EN00(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAnimObject(pDevice,pContext)
{
}

CMonster_EN00::CMonster_EN00(const CMonster_EN00& rhs)
	: CAnimObject(rhs)
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
	m_pModelCom->Set_Loop(true);
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

	if (FAILED(Ready_States()))
		return E_FAIL;

	m_pModelCom->Set_Animation(rand() % 20);

	// 클라 테스트용 m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(rand() % 20, 3.f, rand() % 20, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(rand() % 20, 0.f, rand() % 20, 1.f));

	return S_OK;
}

void CMonster_EN00::Priority_Tick(_float fTimeDelta)
{
	m_pStateCom->Priority_Tick(fTimeDelta);

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Priority_Tick(fTimeDelta);
	}
}

void CMonster_EN00::Tick(_float fTimeDelta)
{
	m_pStateCom->Tick(fTimeDelta);

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Tick(fTimeDelta);
	}

	m_pModelCom->Play_Animation(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CMonster_EN00::Late_Tick(_float fTimeDelta)
{
	m_pStateCom->Late_Tick(fTimeDelta);

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Late_Tick(fTimeDelta);
	}

	CCollider* pTargetCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider")));
	
	if (m_pColliderCom->Collision(pTargetCollider))
	{
		int i = 0;
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

	auto iter = Out_Json["Component"].find("Model");
	Out_Json["Component"].erase(iter);
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

	BoundingDesc.vExtents = _float3(0.5f, 0.7f, 0.5f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_StateMachine"),
		TEXT("Com_StateMachine"), reinterpret_cast<CComponent**>(&m_pStateCom), this)))
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

HRESULT CMonster_EN00::Ready_States()
{
	if (FAILED(m_pStateCom->Add_State(TEXT("EN00State_Idle"), CEN00State_Idle::Create(this))))
		return E_FAIL;
	
	return S_OK;
}

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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	
	return S_OK;
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

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
