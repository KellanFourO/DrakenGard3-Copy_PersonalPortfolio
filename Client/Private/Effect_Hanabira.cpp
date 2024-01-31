#include "stdafx.h"
#include "Effect_Hanabira.h"

#include "GameInstance.h"

CEffect_Hanabira::CEffect_Hanabira(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CNonAnimObject(pDevice, pContext)
{
}

CEffect_Hanabira::CEffect_Hanabira(const CEffect_Hanabira& rhs)
	: CNonAnimObject(rhs)
{

}

HRESULT CEffect_Hanabira::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelID = eLevel;

	

	return S_OK;
}

HRESULT CEffect_Hanabira::Initialize(void* pArg)
{
	HANABIRA_DESC Desc = *(HANABIRA_DESC*)pArg;
	
	if (nullptr == pArg)
		return E_FAIL;

	
	m_RandomNumber = mt19937_64(m_RandomDevice());
	

	__super::Initialize(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Desc.vPos);
	m_pTarget = Desc.pTarget;
	m_eType = Desc.eType;
	m_vCreatePos = Desc.vPos;
	m_vCreateLook = Desc.vLook;
	m_fLifeTime = Desc.fLifeTime;

	return S_OK;
}

void CEffect_Hanabira::Priority_Tick(_float fTimeDelta)
{
	
	
}

void CEffect_Hanabira::Tick(_float fTimeDelta)
{

	m_fAge += fTimeDelta;

	if(m_fAge >= m_fLifeTime)
		Set_Dead();

	if (m_eType == CEffect_Hanabira::HANABIRA_ORBIT)
	{
		m_pTransformCom->Look_At(m_pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		m_pTransformCom->Go_Right(fTimeDelta);
	}
	else if (m_eType == CEffect_Hanabira::HANABIRA_SPREAD)
	{
		
		m_pTransformCom->Look_At_Dir(-XMLoadFloat4(&m_vCreateLook));
		m_pTransformCom->Go_Straight(fTimeDelta);
		
	}
	
}

void CEffect_Hanabira::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this)))
		return;
}

HRESULT CEffect_Hanabira::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		

		m_pShaderCom->Begin(4);


		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CEffect_Hanabira::Ready_Components()
{
	/* For.Com_Shader */
	//if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_EffectMesh"),
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//	return E_FAIL;

	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Model_Effect_Hanabira"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CEffect_Hanabira::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamDirection", &m_pGameInstance->Get_CamDir(), sizeof(_float4))))
	//	return E_FAIL;

	return S_OK;
}


CEffect_Hanabira* CEffect_Hanabira::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CEffect_Hanabira* pInstance = new CEffect_Hanabira(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CEffect_Hanabira");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_Hanabira::Clone(void* pArg)
{
	CEffect_Hanabira* pInstance = new CEffect_Hanabira(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Hanabira");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Hanabira::Free()
{
	__super::Free();

}
