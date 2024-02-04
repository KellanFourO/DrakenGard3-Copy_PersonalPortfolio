#include "stdafx.h"
#include "..\Public\Effect_BossPear.h"

#include "GameInstance.h"


CEffect_BossPear::CEffect_BossPear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAlphaObject(pDevice, pContext)
{

}

CEffect_BossPear::CEffect_BossPear(const CEffect_BossPear& rhs)
	: CAlphaObject(rhs)
{
}

HRESULT CEffect_BossPear::Initialize_Prototype()
{
	/* ������ü�� �ʱ�ȭ������ �����Ѵ�. */
	/* 1.�����κ��� ���� �޾ƿͼ� �ʱ�ȭ�Ѵ� .*/
	/* 2.����������� ���� ���� �޾ƿ´�.*/
	//int*		pData_int = new int(3);

	//void**		pData = (void**)&pData_int;

	return S_OK;
}

HRESULT CEffect_BossPear::Initialize(void* pArg)
{

	m_tInfo = *(BOSSPEAR_DESC*)pArg;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// void* pArg : �����Ͽ� ��ü�� �����Ҷ� �������� ä���� ���� �����͸� �߰������� �� �ʱ�ȭ���ֱ����ؼ�. 
	if (FAILED(Ready_Components()))
		return E_FAIL;
	m_fLifeTime = m_tInfo.fLifeTime;
	m_tInfo.vCreatePos.y += 2.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_tInfo.vCreatePos));

	m_pTransformCom->Look_At_Dir(XMLoadFloat3(&m_tInfo.vCreateLook));
	
	m_pTransformCom->Set_Scaling(40.f, 40.f, 1.f);

	return S_OK;
}

void CEffect_BossPear::Priority_Tick(_float fTimeDelta)
{
	int a = 10;


}

void CEffect_BossPear::Tick(_float fTimeDelta)
{
	m_fFrame += 26.f * fTimeDelta;

	if (m_fFrame >= 26.0f)
		m_fFrame = 0.f;

	m_fTimeAcc += fTimeDelta;
	m_fAge += fTimeDelta;


	if(m_fAge > m_fLifeTime)
		Set_Dead();

	if (m_fTimeAcc > m_tInfo.fAddTime)
	{
		_float3 vScale = m_pTransformCom->Get_Scaled();

		vScale.x += m_tInfo.fTickAddScaleX;
		vScale.y += m_tInfo.fTickAddScaleY;

		m_pTransformCom->Set_Scaling(vScale.x, vScale.y, vScale.z);

		m_fTimeAcc = 0.f;
	}

	m_pTransformCom->Go_Straight(fTimeDelta);

}

void CEffect_BossPear::Late_Tick(_float fTimeDelta)
{
	Compute_CamDistance();

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this)))
		return;
}

HRESULT CEffect_BossPear::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_bool bCustomColor = true;
	m_pShaderCom->Bind_RawValue("g_bCustomColor", &bCustomColor, sizeof(_bool));
	_float4 vColor = { 1.f, 1.f, 1.f, 1.f};
	m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));

	/* �� �Τ��̴��� 0��° �н��� �׸�����. */
	m_pShaderCom->Begin(2);

	/* ���� �׸������ϴ� ����, �ΉK�������۸� ��ġ�� ���ε���. */
	m_pVIBufferCom->Bind_VIBuffers();

	/* ���ε��� ����, �ε����� �׷�. */
	m_pVIBufferCom->Render();

	bCustomColor = false;

	m_pShaderCom->Bind_RawValue("g_bCustomColor", &bCustomColor, sizeof(_bool));

	return S_OK;
}

HRESULT CEffect_BossPear::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_Dash"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_BossPear::Bind_ShaderResources()
{

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", _uint(m_fFrame))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
		return E_FAIL;

	
		

	return S_OK;
}

CEffect_BossPear* CEffect_BossPear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_BossPear* pInstance = new CEffect_BossPear(pDevice, pContext);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_BossPear");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_BossPear::Clone(void* pArg)
{
	CEffect_BossPear* pInstance = new CEffect_BossPear(*this);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_BossPear");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_BossPear::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}

