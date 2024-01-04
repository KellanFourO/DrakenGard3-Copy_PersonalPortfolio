#include "stdafx.h"
#include "Environment_AmmorBox.h"
#include "GameInstance.h"

CEnvironment_AmmorBox::CEnvironment_AmmorBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CNonAnimObject(pDevice, pContext)
{
}

CEnvironment_AmmorBox::CEnvironment_AmmorBox(const CEnvironment_AmmorBox& rhs)
	: CNonAnimObject(rhs)
{
}

HRESULT CEnvironment_AmmorBox::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelID = eLevel;

	return S_OK;
}

HRESULT CEnvironment_AmmorBox::Initialize(void* pArg)
{
	//if(nullptr == pArg)
	//	return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(30.f, 1.f, 30.f, 1.f));

	return S_OK;
}

void CEnvironment_AmmorBox::Priority_Tick(_float fTimeDelta)
{
}

void CEnvironment_AmmorBox::Tick(_float fTimeDelta)
{
}

void CEnvironment_AmmorBox::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CEnvironment_AmmorBox::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CEnvironment_AmmorBox::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Model_Environment_AmmorBox"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnvironment_AmmorBox::Bind_ShaderResources()
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


CEnvironment_AmmorBox* CEnvironment_AmmorBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CEnvironment_AmmorBox* pInstance = new CEnvironment_AmmorBox(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CEnvironment_AmmorBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEnvironment_AmmorBox::Clone(void* pArg)
{
	CEnvironment_AmmorBox* pInstance = new CEnvironment_AmmorBox(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEnvironment_AmmorBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEnvironment_AmmorBox::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
