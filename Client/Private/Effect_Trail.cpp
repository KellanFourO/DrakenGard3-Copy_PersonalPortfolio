#include "stdafx.h"
#include "Effect_Trail.h"
#include "GameInstance.h"
#include "VIBuffer_Trail.h"

CEffect_Trail::CEffect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eCurrentLevel)
	: CAlphaObject(pDevice, pContext)
	, m_eCurrentLevel(eCurrentLevel)
{
}

CEffect_Trail::CEffect_Trail(const CEffect_Trail& rhs)
	: CAlphaObject(rhs)
	, m_eCurrentLevel(rhs.m_eCurrentLevel)
{
}

HRESULT CEffect_Trail::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect_Trail::Initialize(void* pArg)//trail을 사용하는 객체로부터 traildesc를 받는다.
{
	m_tTrailDesc = *(EFFECT_TRAIL_DESC*)pArg;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	
	
	return S_OK;
}



void CEffect_Trail::Tick(_float fTimeDelta, _fmatrix OwnerWorldMatrix)
{
	if (false == m_bTrailOn)
		return;

	m_pVIBuffer->Update(fTimeDelta, OwnerWorldMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, OwnerWorldMatrix.r[3]);
	
}



void CEffect_Trail::Late_Tick(_float fTimeDelta)
{
	Compute_CamDistance();

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this)))
		return;
}

HRESULT CEffect_Trail::Render()
{
	if(false == m_bTrailOn)
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 셰ㅒ이더에 0번째 패스로 그릴꺼야. */
	m_pShaderCom->Begin(6);

	/* 내가 그릴려고하는 정점, 인덷ㄱ스버퍼를 장치에 바인딩해. */
	m_pVIBuffer->Bind_VIBuffers();

	/* 바인딩된 정점, 인덱스를 그려. */
	m_pVIBuffer->Render();


	return S_OK;
}



HRESULT CEffect_Trail::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CVIBuffer_Trail::TRAIL_DESC BufferDesc;

	BufferDesc.iMaxCount = m_tTrailDesc.iMaxCount;
	BufferDesc.vStartPos = m_tTrailDesc.vStartPos;
	BufferDesc.vEndPos = m_tTrailDesc.vEndPos;


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(m_eCurrentLevel, TEXT("Prototype_Component_VIBuffer_Trail"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer), &BufferDesc)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_eCurrentLevel, TEXT("Prototype_Component_Texture_PlayerSwordTrail"),
		TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pDiffuseTextureCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_eCurrentLevel, TEXT("Prototype_Component_Texture_PlayerSwordTrailMask2"),
		TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_eCurrentLevel, TEXT("Prototype_Component_Texture_PlayerSwordTrailNoise"),
		TEXT("Com_NoiseTexture"), reinterpret_cast<CComponent**>(&m_pNoiseTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CEffect_Trail::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pDiffuseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pDiffuseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;

	if (FAILED(m_pDiffuseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture")))
		return E_FAIL;

	return S_OK;
}


void CEffect_Trail::Reset_Points()
{
	m_pVIBuffer->Reset_Points(m_pTransformCom->Get_WorldMatrix());
}


CEffect_Trail* CEffect_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eCurrentLevel)
{
	CEffect_Trail* pInstance = new CEffect_Trail(pDevice, pContext, eCurrentLevel);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Trail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_Trail::Clone(void* pArg)
{
	CEffect_Trail* pInstance = new CEffect_Trail(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Trail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Trail::Free()
{
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pDiffuseTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pShaderCom);
}