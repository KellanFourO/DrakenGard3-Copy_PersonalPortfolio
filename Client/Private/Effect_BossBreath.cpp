#include "stdafx.h"
#include "Effect_BossBreath.h"

#include "GameInstance.h"
#include "Effect_BornFire.h"

CEffect_BossBreath::CEffect_BossBreath(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CNonAnimObject(pDevice, pContext)
{
}

CEffect_BossBreath::CEffect_BossBreath(const CEffect_BossBreath& rhs)
	: CNonAnimObject(rhs)
{

}

HRESULT CEffect_BossBreath::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelID = eLevel;

	

	return S_OK;
}

HRESULT CEffect_BossBreath::Initialize(void* pArg)
{
	BOSSBREATH_DESC Desc = *(BOSSBREATH_DESC*)pArg;
	
	if (nullptr == pArg)
		return E_FAIL;

	
	m_RandomNumber = mt19937_64(m_RandomDevice());
	

	__super::Initialize(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Desc.vPos);
	m_pTarget = Desc.pTarget;
	m_vCreatePos = Desc.vPos;
	m_vCreateLook = Desc.vLook;
	m_fLifeTime = Desc.fLifeTime;



	m_pTransformCom->Look_At_Dir(XMLoadFloat4(&m_vCreateLook));
	
	return S_OK;
}

void CEffect_BossBreath::Priority_Tick(_float fTimeDelta)
{
	
	
}

void CEffect_BossBreath::Tick(_float fTimeDelta)
{

	m_fAge += fTimeDelta;
	m_vAddUVPos.x += fTimeDelta;
	m_vAddUVPos.y += fTimeDelta;

	if(m_fAge >= m_fLifeTime)
		Set_Dead();


	//CreateFireEffect();

	m_pTransformCom->Go_Straight(fTimeDelta);
	
}

void CEffect_BossBreath::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this)))
		return;
}

HRESULT CEffect_BossBreath::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		

		m_pShaderCom->Begin(5);


		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CEffect_BossBreath::Ready_Components()
{
	/* For.Com_Shader */
	//if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_EffectMesh"),
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//	return E_FAIL;

	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Model_Effect_BossFire"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Texture_BossFireNoise"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	
	//m_pTextureCom
	return S_OK;
}

HRESULT CEffect_BossBreath::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture")))
		return E_FAIL;

	

	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vAddUVPos", &m_vAddUVPos, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}


void CEffect_BossBreath::CreateFireEffect()
{
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4 vRandomPos = m_pTransformCom->Get_RandomPositionAroundCenter(vMyPos, XMConvertToRadians(360.f));

	m_pGameInstance->Add_CloneObject(m_eCurrentLevelID, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_BornFire"), &vRandomPos);
}

CEffect_BossBreath* CEffect_BossBreath::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CEffect_BossBreath* pInstance = new CEffect_BossBreath(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CEffect_BossBreath");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_BossBreath::Clone(void* pArg)
{
	CEffect_BossBreath* pInstance = new CEffect_BossBreath(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_BossBreath");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_BossBreath::Free()
{
	__super::Free();

}
