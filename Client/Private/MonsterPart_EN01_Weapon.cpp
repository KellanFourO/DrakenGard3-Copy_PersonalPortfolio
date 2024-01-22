#include "stdafx.h"
#include "MonsterPart_EN01_Weapon.h"
#include "GameInstance.h"
#include "Bone.h"
#include "EN01_Arrow.h"

CMonsterPart_EN01_Weapon::CMonsterPart_EN01_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice,pContext)
{
}

CMonsterPart_EN01_Weapon::CMonsterPart_EN01_Weapon(const CMonsterPart_EN01_Weapon& rhs)
	: CPartObject(rhs)
{
}

HRESULT CMonsterPart_EN01_Weapon::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelIndex = eLevel;
	m_strName = "CMonsterPart_EN01_Weapon";
	return S_OK;
}

HRESULT CMonsterPart_EN01_Weapon::Initialize(void* pArg)
{
	m_strName = "CMonsterPart_EN01_Weapon";
	
	m_pParentTransformCom = ((PART_DESC*)pArg)->m_pParentTransform;


	if(FAILED(AddRefIfNotNull(m_pParentTransformCom)))
		return E_FAIL;

	m_pSocketBone = ((PART_DESC*)pArg)->m_pSocketBone;

	if (FAILED(AddRefIfNotNull(m_pSocketBone)))
		return E_FAIL;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(__super::Ready_Components(m_eCurrentLevelIndex, TEXT("Prototype_Component_Shader_Model"), TEXT("Prototype_Component_Model_Monster_EN01_Weapon"))))
		return E_FAIL;


	m_pTransformCom->Set_Scaling(1.f, 1.f, 1.f);
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.0f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.7f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CMonsterPart_EN01_Weapon::Priority_Tick(_float fTimeDelta)
{
	
}

void CMonsterPart_EN01_Weapon::Tick(_float fTimeDelta)
{
	
}

void CMonsterPart_EN01_Weapon::Late_Tick(_float fTimeDelta)
{
	_matrix		SocketMatrix = m_pSocketBone->Get_CombinedTransformationMatrix();

	////TODO ���� �����ϰ��� ������ 1������ ����� �ڵ�.
	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}
	//TODO �Ʒ� �ڵ尡 ���� ������Ʈ�� ��
	//! ���� ������Ʈ�� �����Ʈ������ �θ��� ������ı������� ����������Ѵ�. ���� ����������Ʈ�� �����Ʈ������ �׵��� ��쿡�� �θ���ؿ� �پ����� ��.
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * m_pParentTransformCom->Get_WorldMatrix());


	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CMonsterPart_EN01_Weapon::Render()
{
	//#���͸𵨷���

	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	//TODO Ŭ�󿡼� ���� �޽� ������ �޾ƿͼ� ��ȸ�ϸ鼭 ���̴� ���ε�������.

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		//TODO Bind_BoneMatrices �Լ��� �ִϸ��̼��� �ִ� ���� ��쿡�� �����ؾ��ϴ� �Լ��̴� ������
		//m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i); 

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(0); //! ���̴��� �����ְ� ��� ȣ���ϴ� �� ��������

		m_pModelCom->Render(i);
	}

	return S_OK;
}


CGameObject* CMonsterPart_EN01_Weapon::CreateNormalArrow()
{
	CEN01_Arrow::EN01_ARROWDESC Desc;

	Desc.fDmg = 5.f;
	Desc.fDeadTime = 5.f;
	Desc.fRange = 30.f;
	XMStoreFloat4(&Desc.vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	Desc.OwnerWorldMatrix = m_WorldMatrix;
	Desc.fSpeedPerSec = 15.f;
	Desc.fRotationPerSec = XMConvertToRadians(150.f);
	Desc.eArrowType = CEN01_Arrow::ARROW_NORMAL;


	CGameObject* pGameObject = nullptr;

	if (FAILED(m_pGameInstance->Add_CloneObject(m_eCurrentLevelIndex, TEXT("Layer_Bullet"), TEXT("Prototype_GameObject_Monster_EN01_Arrow"), &Desc, &pGameObject)))
		_int i = 0;

	return pGameObject;

	
}

CGameObject* CMonsterPart_EN01_Weapon::CreateParabolicArrow()
{
	CEN01_Arrow::EN01_ARROWDESC Desc;

	Desc.fDmg = 5.f;
	Desc.fDeadTime = 5.f;
	Desc.fRange = 30.f;
	XMStoreFloat4(&Desc.vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	Desc.OwnerWorldMatrix = m_WorldMatrix;
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(150.f);
	Desc.eArrowType = CEN01_Arrow::ARROW_PARABOLIC;


	CGameObject* pGameObject = nullptr;

	if (FAILED(m_pGameInstance->Add_CloneObject(m_eCurrentLevelIndex, TEXT("Layer_Bullet"), TEXT("Prototype_GameObject_Monster_EN01_Arrow"), &Desc, &pGameObject)))
		_int i = 0;

	return pGameObject;
}

CGameObject* CMonsterPart_EN01_Weapon::CreateChargeNormalArrow()
{
	CEN01_Arrow::EN01_ARROWDESC Desc;

	Desc.fDmg = 10.f;
	Desc.fDeadTime = 5.f;
	Desc.fRange = 30.f;
	XMStoreFloat4(&Desc.vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	Desc.OwnerWorldMatrix = m_WorldMatrix;
	Desc.fSpeedPerSec = 20.f;
	Desc.fRotationPerSec = XMConvertToRadians(150.f);
	Desc.eArrowType = CEN01_Arrow::ARROW_C_NORMAL;

	CGameObject* pGameObject = nullptr;

	if (FAILED(m_pGameInstance->Add_CloneObject(m_eCurrentLevelIndex, TEXT("Layer_Bullet"), TEXT("Prototype_GameObject_Monster_EN01_Arrow"), &Desc, &pGameObject)))
		_int i = 0;

	return pGameObject;
}

CGameObject* CMonsterPart_EN01_Weapon::CreateChargeParabolicArrow()
{
	CEN01_Arrow::EN01_ARROWDESC Desc;

	Desc.fDmg = 10.f;
	Desc.fDeadTime = 5.f;
	Desc.fRange = 30.f;
	XMStoreFloat4(&Desc.vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	Desc.OwnerWorldMatrix = m_WorldMatrix;
	Desc.fSpeedPerSec = 15.f;
	Desc.fRotationPerSec = XMConvertToRadians(150.f);
	Desc.eArrowType = CEN01_Arrow::ARROW_C_PARABOLIC;


	CGameObject* pGameObject = nullptr;

	if (FAILED(m_pGameInstance->Add_CloneObject(m_eCurrentLevelIndex, TEXT("Layer_Bullet"), TEXT("Prototype_GameObject_Monster_EN01_Arrow"), &Desc, &pGameObject)))
		_int i = 0;

	return pGameObject;
}

void CMonsterPart_EN01_Weapon::Write_Json(json& Out_Json)
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

void CMonsterPart_EN01_Weapon::Load_FromJson(const json& In_Json)
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

void CMonsterPart_EN01_Weapon::Init_Desc()
{
	//m_pStatus.lock()->Init_Status(&m_tLinkStateDesc);
}


CMonsterPart_EN01_Weapon* CMonsterPart_EN01_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CMonsterPart_EN01_Weapon* pInstance = new CMonsterPart_EN01_Weapon(pDevice, pContext);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CMonsterPart_EN01_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMonsterPart_EN01_Weapon::Clone(void* pArg)
{
	CMonsterPart_EN01_Weapon* pInstance = new CMonsterPart_EN01_Weapon(*this);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonsterPart_EN01_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonsterPart_EN01_Weapon::Free()
{
	__super::Free();

}

