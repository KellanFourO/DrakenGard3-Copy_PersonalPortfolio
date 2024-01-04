#include "stdafx.h"
#include "TestSnow.h"
#include "GameInstance.h"

CTestSnow::CTestSnow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CNonAnimObject(pDevice, pContext)
{
}

CTestSnow::CTestSnow(const CTestSnow& rhs)
	: CNonAnimObject(rhs)
{
}

HRESULT CTestSnow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTestSnow::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CTestSnow::Priority_Tick(_float fTimeDelta)
{
}

void CTestSnow::Tick(_float fTimeDelta)
{
}

void CTestSnow::Late_Tick(_float fTimeDelta)
{

	//! ��ī�̹ڽ��� ī�޶� ��ġ�� �������־���Ѵ�. ��� ���̴����� ����������Ʈ�� �����׽�Ʈ�� �����ʰ� ���̱�ϵ� �����ʱ⶧���� �ٸ� ��ü���� �������� �ʰ�, �ڽ��� �׻� �׷����� ���� ��.
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this)))
		return;
}

HRESULT CTestSnow::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_VIBuffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CTestSnow::Ready_Components()
{

	//! For. Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Particle_Rect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//!For. Com_Texture
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	//!For. Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CTestSnow::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

CTestSnow* CTestSnow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTestSnow* pInstance = new CTestSnow(pDevice, pContext);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTestSnow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTestSnow::Clone(void* pArg)
{
	CTestSnow* pInstance = new CTestSnow(*this);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTestSnow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTestSnow::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
