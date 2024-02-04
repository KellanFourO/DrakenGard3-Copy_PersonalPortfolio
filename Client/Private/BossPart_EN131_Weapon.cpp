#include "stdafx.h"
#include "BossPart_EN131_Weapon.h"
#include "GameInstance.h"
#include "Bone.h"
#include "EN131_Breath.h"
#include "Effect_Dust.h"

CBossPart_EN131_Weapon::CBossPart_EN131_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice,pContext)
{
}

CBossPart_EN131_Weapon::CBossPart_EN131_Weapon(const CBossPart_EN131_Weapon& rhs)
	: CPartObject(rhs)
{
}

HRESULT CBossPart_EN131_Weapon::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelIndex = eLevel;
	m_strName = "CBossPart_EN131_Weapon";
	return S_OK;
}

HRESULT CBossPart_EN131_Weapon::Initialize(void* pArg)
{
	m_strName = ((PART_DESC*)pArg)->m_strPartName;
	
	m_pParentTransformCom = ((PART_DESC*)pArg)->m_pParentTransform;


	if(FAILED(AddRefIfNotNull(m_pParentTransformCom)))
		return E_FAIL;

	m_pSocketBone = ((PART_DESC*)pArg)->m_pSocketBone;

	if (FAILED(AddRefIfNotNull(m_pSocketBone)))
		return E_FAIL;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_eCurrentLevelIndex, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	/* For.Com_Collider */
	CBoundingBox_OBB::BOUNDING_OBB_DESC BoundingDesc = {};

	BoundingDesc.vExtents = _float3(0.6f, 0.8f, 0.6f);
	BoundingDesc.vCenter = _float3(0.f, -0.6f, -1.f);
	BoundingDesc.vRotation = _float3(XMConvertToRadians(190.f), 0.f, 0.f);
	BoundingDesc.ePartType = CBoundParent::PARTTYPE_BOUND::PART_WEAPON;

	if (FAILED(__super::Add_Component(m_eCurrentLevelIndex, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	m_pColliderCom->Set_PartType(CCollider::PART_WEAPON);

	m_pTransformCom->Set_Scaling(1.f, 1.f, 1.f);
	m_pColliderCom->OffCollider();
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.0f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.7f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CBossPart_EN131_Weapon::Priority_Tick(_float fTimeDelta)
{
	
}

void CBossPart_EN131_Weapon::Tick(_float fTimeDelta)
{
	
}

void CBossPart_EN131_Weapon::Late_Tick(_float fTimeDelta)
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

	m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CBossPart_EN131_Weapon::Render()
{
	//#몬스터모델렌더

	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(6); 

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG

	return S_OK;
}


CGameObject* CBossPart_EN131_Weapon::CreateBreath(_fvector vTargetPos)
{
	_vector vPos = vTargetPos;
	vPos.m128_f32[3] = 1.f;

	CEN131_Breath::EN131_BREATHDESC Desc;
	
	Desc.fDmg = 5.f;
	Desc.fDeadTime = 5.f;
	Desc.fRange = 30.f;
	XMStoreFloat4(&Desc.vLook, m_pParentTransformCom->Get_State(CTransform::STATE_LOOK));
	//XMStoreFloat4(&Desc.vLook, XMLoadFloat4x4(&m_WorldMatrix).r[3]);
	//XMStoreFloat4(&Desc.vLook, vPos - XMLoadFloat4x4(&m_WorldMatrix).r[3]);
	//fDesc.vLook = { m_WorldMatrix._31, m_WorldMatrix._32, m_WorldMatrix._33, 0.f };
	//XMStoreFloat4(&Desc.vLook, XMLoadFloat4x4(&m_WorldMatrix).r[2]);
	//XMStoreFloat4(&Desc.vLook,m_pParentTransformCom->Get_State(CTransform::STATE_LOOK));
	Desc.OwnerWorldMatrix = m_WorldMatrix;
	Desc.fSpeedPerSec = 15.f;
	Desc.fRotationPerSec = XMConvertToRadians(150.f);
	
	

	if (m_strName == "L_HEAD")
		Desc.eBreathType = CEN131_Breath::BREATH_LEFT;
	else if (m_strName == "C_HEAD")
		Desc.eBreathType = CEN131_Breath::BREATH_CENTER;
	else if (m_strName == "R_HEAD")
		Desc.eBreathType = CEN131_Breath::BREATH_RIGHT;
	
	//XMStoreFloat4(&Desc.vTargetPos, vTargetPos);

	Desc.vParentLook;
	XMStoreFloat4(&Desc.vParentLook, m_pParentTransformCom->Get_State(CTransform::STATE_LOOK));
	
	CGameObject* pGameObject = nullptr;

	if (FAILED(m_pGameInstance->Add_CloneObject(m_eCurrentLevelIndex, TEXT("Layer_Bullet"), TEXT("Prototype_GameObject_EN131_Breath"), &Desc, &pGameObject)))
		_int i = 0;

	return pGameObject;

	
}

void CBossPart_EN131_Weapon::CreateStringEffect()
{
	CEffect_Dust::EFFECT_DESC Desc;

	Desc.fLifeTime = 1.f;
	Desc.fPlaySpeed = 0.05f;

	XMStoreFloat3(&Desc.vDir, XMLoadFloat4x4(&m_WorldMatrix).r[2]);
	XMStoreFloat4(&Desc.vCreatePos, XMLoadFloat4x4(&m_WorldMatrix).r[3]);
	Desc.vScale = _float3(3.f, 3.0f, 1.f);

	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Dust"), &Desc);
}



void CBossPart_EN131_Weapon::Write_Json(json& Out_Json)
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

void CBossPart_EN131_Weapon::Load_FromJson(const json& In_Json)
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

void CBossPart_EN131_Weapon::Init_Desc()
{
	//m_pStatus.lock()->Init_Status(&m_tLinkStateDesc);
}


CBossPart_EN131_Weapon* CBossPart_EN131_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CBossPart_EN131_Weapon* pInstance = new CBossPart_EN131_Weapon(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CBossPart_EN131_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBossPart_EN131_Weapon::Clone(void* pArg)
{
	CBossPart_EN131_Weapon* pInstance = new CBossPart_EN131_Weapon(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBossPart_EN131_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossPart_EN131_Weapon::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);

}

