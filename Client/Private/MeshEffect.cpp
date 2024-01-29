#include "stdafx.h"
#include "MeshEffect.h"

#include "GameInstance.h"

CMeshEffect::CMeshEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CNonAnimObject(pDevice, pContext)
{
}

CMeshEffect::CMeshEffect(const CMeshEffect& rhs)
	: CNonAnimObject(rhs)
{

}

HRESULT CMeshEffect::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelID = eLevel;

	
	return S_OK;
}

HRESULT CMeshEffect::Initialize(void* pArg)
{
	CMeshEffect::MESH_EFFECTDESC Desc = {};
	
	if (nullptr == pArg)
		return E_FAIL;

	Desc = *(MESH_EFFECTDESC*)pArg;

	//typedef struct tagMesh_EffectDesc : public CGameObject::GAMEOBJECT_DESC
	//{
	//	wstring	strModelTag;
	//	_uint	iShaderPassIndex;
	//	_float3 vCenter;
	//	_float  fRange;
	//	_float2 vSpeed;
	//	_float2 vScale;
	//	_float3 vRotation;
	//	_float4	vColor;
	//	_float2 vLifeTime;
	//	_float	fAge;
	//
	//}MESH_EFFECTDESC;

	m_strModelTag = Desc.strModelTag;
	m_iShaderPassIndex = Desc.iShaderPassIndex;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	_vector vPos = XMLoadFloat3(&Desc.vCenter);
	vPos.m128_f32[3] = 1.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_fLifeTime = Desc.vLifeTime.y;
	//m_pTransformCom->Set_WorldFloat4x4(m_WorldMatrix);

	if (FAILED(Ready_Components()))
		return E_FAIL;


	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(15.f, 6.f, 30.f, 1.f));

	return S_OK;
}

void CMeshEffect::Priority_Tick(_float fTimeDelta)
{
	
	
}

void CMeshEffect::Tick(_float fTimeDelta)
{

	m_fAge += fTimeDelta;

	if(m_fAge >= m_fLifeTime)
		Set_Dead();
}

void CMeshEffect::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CMeshEffect::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		m_pShaderCom->Begin(0);


		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CMeshEffect::Ready_Components()
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

HRESULT CMeshEffect::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	

	return S_OK;
}


CMeshEffect* CMeshEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CMeshEffect* pInstance = new CMeshEffect(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CMeshEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMeshEffect::Clone(void* pArg)
{
	CMeshEffect* pInstance = new CMeshEffect(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMeshEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMeshEffect::Free()
{
	__super::Free();

}
