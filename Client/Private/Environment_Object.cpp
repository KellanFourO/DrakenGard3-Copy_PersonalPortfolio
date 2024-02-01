#include "stdafx.h"
#include "Environment_Object.h"

#include "GameInstance.h"

CEnvironment_Object::CEnvironment_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CNonAnimObject(pDevice, pContext)
{
}

CEnvironment_Object::CEnvironment_Object(const CEnvironment_Object& rhs)
	: CNonAnimObject(rhs)
	, m_pNavigationCom(rhs.m_pNavigationCom)
{
	Safe_AddRef(m_pNavigationCom);
}

HRESULT CEnvironment_Object::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelID = eLevel;

	
	//! For.Com_Navigation
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnvironment_Object::Initialize(void* pArg)
{
	CEnvironment_Object::ENVIRONMENT_DESC Desc = {};

	if (nullptr != pArg)
		Desc = *(ENVIRONMENT_DESC*)pArg;

	m_strModelTag = Desc.strModelTag;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_WorldFloat4x4(Desc.WorldMatrix);
	//m_pTransformCom->Set_WorldFloat4x4(m_WorldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;


	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(15.f, 6.f, 30.f, 1.f));

	return S_OK;
}

void CEnvironment_Object::Priority_Tick(_float fTimeDelta)
{
	
	m_pNavigationCom->Update(XMMatrixIdentity());
}

void CEnvironment_Object::Tick(_float fTimeDelta)
{

}

void CEnvironment_Object::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
	//if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this)))
	//	return;
}

HRESULT CEnvironment_Object::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(0);


		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	m_pNavigationCom->Render();
#endif 


	return S_OK;
}

HRESULT CEnvironment_Object::Render_Shadow()
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

		m_pShaderCom->Begin(6); //! 셰이더에 던져주고 비긴 호출하는 걸 잊지말자

		m_pModelCom->Render(i);
	}


	return S_OK;
}

HRESULT CEnvironment_Object::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, m_strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CEnvironment_Object::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	

	return S_OK;
}


CEnvironment_Object* CEnvironment_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CEnvironment_Object* pInstance = new CEnvironment_Object(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CEnvironment_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEnvironment_Object::Clone(void* pArg)
{
	CEnvironment_Object* pInstance = new CEnvironment_Object(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEnvironment_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEnvironment_Object::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	
}
