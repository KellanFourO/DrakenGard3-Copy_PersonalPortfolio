#include "stdafx.h"
#include "MonsterPart_EN00_Weapon.h"
#include "GameInstance.h"
#include "Bone.h"

CMonsterPart_EN00_Weapon::CMonsterPart_EN00_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice,pContext)
{
}

CMonsterPart_EN00_Weapon::CMonsterPart_EN00_Weapon(const CMonsterPart_EN00_Weapon& rhs)
	: CPartObject(rhs)
{
}

HRESULT CMonsterPart_EN00_Weapon::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelIndex = eLevel;
	m_strName = "CMonsterPart_EN00_Weapon";
	return S_OK;
}

HRESULT CMonsterPart_EN00_Weapon::Initialize(void* pArg)
{
	m_strName = "CMonsterPart_EN00_Weapon";
	
	m_pParentTransformCom = ((PART_DESC*)pArg)->m_pParentTransform;


	if(FAILED(AddRefIfNotNull(m_pParentTransformCom)))
		return E_FAIL;

	m_pSocketBone = ((PART_DESC*)pArg)->m_pSocketBone;

	if (FAILED(AddRefIfNotNull(m_pSocketBone)))
		return E_FAIL;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(__super::Ready_Components(m_eCurrentLevelIndex, TEXT("Prototype_Component_Shader_Model"), TEXT("Prototype_Component_Model_Monster_EN00_Weapon"))))
		return E_FAIL;

	/* For.Com_Collider */
	CBoundingBox_OBB::BOUNDING_OBB_DESC BoundingDesc = {};

	BoundingDesc.vExtents = _float3(0.3f, 0.5f, 0.3f);
	BoundingDesc.vCenter = _float3(0.f, 0.f, -0.6f);
	BoundingDesc.vRotation = _float3(XMConvertToRadians(90.f), 0.f, 0.f);
	BoundingDesc.ePartType = CBoundParent::PARTTYPE_BOUND::PART_WEAPON;

	if (FAILED(__super::Add_Component(m_eCurrentLevelIndex, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	m_pColliderCom->Set_PartType(CCollider::PART_WEAPON);

	m_pTransformCom->Set_Scaling(1.f, 1.f, 1.f);
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.0f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.7f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CMonsterPart_EN00_Weapon::Priority_Tick(_float fTimeDelta)
{
	
}

void CMonsterPart_EN00_Weapon::Tick(_float fTimeDelta)
{
	
}

void CMonsterPart_EN00_Weapon::Late_Tick(_float fTimeDelta)
{
	_matrix		SocketMatrix = m_pSocketBone->Get_CombinedTransformationMatrix();

	////TODO 기존 스케일값을 강제로 1값으로 만드는 코드.
	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}
	//TODO 아래 코드가 파츠 오브젝트의 꽃
	//! 파츠 오브젝트의 월드매트릭스는 부모의 월드행렬기준으로 움직여줘야한다. 만약 파츠오브젝트의 월드매트릭스가 항등일 경우에도 부모기준에 붙어있을 것.
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * m_pParentTransformCom->Get_WorldMatrix());

	
	
	//XMMATRIX rotatedWorldMatrix = XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMLoadFloat4x4(&m_WorldMatrix);

	m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CMonsterPart_EN00_Weapon::Render()
{
	//#몬스터모델렌더

	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	//TODO 클라에서 모델의 메시 개수를 받아와서 순회하면서 셰이더 바인딩해주자.

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		//TODO Bind_BoneMatrices 함수는 애니메이션이 있는 모델일 경우에만 수행해야하는 함수이니 빼주자
		//m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i); 

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(0); //! 셰이더에 던져주고 비긴 호출하는 걸 잊지말자

		m_pModelCom->Render(i);
	}


#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG

	return S_OK;
}

void CMonsterPart_EN00_Weapon::Write_Json(json& Out_Json)
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

void CMonsterPart_EN00_Weapon::Load_FromJson(const json& In_Json)
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

void CMonsterPart_EN00_Weapon::Init_Desc()
{
	//m_pStatus.lock()->Init_Status(&m_tLinkStateDesc);
}


CMonsterPart_EN00_Weapon* CMonsterPart_EN00_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CMonsterPart_EN00_Weapon* pInstance = new CMonsterPart_EN00_Weapon(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CMonsterPart_EN00_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMonsterPart_EN00_Weapon::Clone(void* pArg)
{
	CMonsterPart_EN00_Weapon* pInstance = new CMonsterPart_EN00_Weapon(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonsterPart_EN00_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonsterPart_EN00_Weapon::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);

}

