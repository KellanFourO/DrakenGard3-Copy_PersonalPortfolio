#include "stdafx.h"
#include "..\Public\PlayerPart_Body.h"
#include "GameInstance.h"
#include "Bone.h"

CPlayerPart_Body::CPlayerPart_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice,pContext)
{
}

CPlayerPart_Body::CPlayerPart_Body(const CPlayerPart_Body& rhs)
	: CPartObject(rhs)
{
}

HRESULT CPlayerPart_Body::Initialize_Prototype(LEVEL eLevel)
{
	m_strName = "CPlayerPart_Body";

	m_eCurrentLevelID = eLevel;

	return S_OK;
}

HRESULT CPlayerPart_Body::Initialize(void* pArg)
{
	

	m_pParentTransformCom = ((PART_DESC*)pArg)->m_pParentTransform;

	if (FAILED(AddRefIfNotNull(m_pParentTransformCom)))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(__super::Ready_Components(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Prototype_Component_Model_Fiona"))))
		return E_FAIL;

	return S_OK;
}

void CPlayerPart_Body::Priority_Tick(_float fTimeDelta)
{
}

void CPlayerPart_Body::Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta, true);
}

void CPlayerPart_Body::Late_Tick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix());

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CPlayerPart_Body::Render()
{
	//#���͸𵨷���
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	//TODO Ŭ�󿡼� ���� �޽� ������ �޾ƿͼ� ��ȸ�ϸ鼭 ���̴� ���ε�������.

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(0); //! ���̴��� �����ְ� ��� ȣ���ϴ� �� ��������

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CPlayerPart_Body::Write_Json(json& Out_Json)
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

void CPlayerPart_Body::Load_FromJson(const json& In_Json)
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

void CPlayerPart_Body::Init_Desc()
{
	//m_pStatus.lock()->Init_Status(&m_tLinkStateDesc);
}


CPlayerPart_Body* CPlayerPart_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CPlayerPart_Body* pInstance = new CPlayerPart_Body(pDevice, pContext);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CPlayerPart_Body");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayerPart_Body::Clone(void* pArg)
{
	CPlayerPart_Body* pInstance = new CPlayerPart_Body(*this);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayerPart_Body");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerPart_Body::Free()
{
	__super::Free();

}
