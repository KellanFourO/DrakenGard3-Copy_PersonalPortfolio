#include "stdafx.h"
#include "..\Public\Particle_Object.h"

#include "GameInstance.h"


CParticle_Object::CParticle_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CNonAnimObject(pDevice, pContext)
{

}

CParticle_Object::CParticle_Object(const CParticle_Object & rhs)
	: CNonAnimObject(rhs)
{
}

HRESULT CParticle_Object::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelID = eLevel;
	/* ������ü�� �ʱ�ȭ������ �����Ѵ�. */
	/* 1.�����κ��� ���� �޾ƿͼ� �ʱ�ȭ�Ѵ� .*/
	/* 2.����������� ���� ���� �޾ƿ´�.*/
	//int*		pData_int = new int(3);

	//void**		pData = (void**)&pData_int;

	return S_OK;
}

HRESULT CParticle_Object::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	m_tParticleDesc = *(PARTICLE_DESC*)pArg;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;
	

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 10.f, 0.f, 1.f));

	return S_OK;
}

void CParticle_Object::Priority_Tick(_float fTimeDelta)
{
	

	
}

void CParticle_Object::Tick(_float fTimeDelta)
{
	if(true ==m_bParticleStart)
	 m_pVIBufferCom->Update(fTimeDelta);


}

void CParticle_Object::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this)))
		return;
}

HRESULT CParticle_Object::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* �� �Τ��̴��� 0��° �н��� �׸�����. */
	m_pShaderCom->Begin(m_tParticleDesc.iShaderPathIndex);

	/* ���� �׸������ϴ� ����, �ΉK�������۸� ��ġ�� ���ε���. */
	m_pVIBufferCom->Bind_VIBuffers();

	/* ���ε��� ����, �ε����� �׷�. */
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CParticle_Object::Ready_Components(void* pArg)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_Particle_Point"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CVIBuffer_Particle_Point::PARTICLE_POINT_DESC ParticleDesc = *(CVIBuffer_Particle_Point::PARTICLE_POINT_DESC*)pArg;

	/* For.Com_VIBuffer */
	ParticleDesc.vCenter = m_tParticleDesc.vCenter;
	ParticleDesc.fRange = m_tParticleDesc.fRange;
	ParticleDesc.vScale = m_tParticleDesc.vScale;
	ParticleDesc.vSpeed = m_tParticleDesc.vSpeed;
	ParticleDesc.vRotation = m_tParticleDesc.vRotation;
	ParticleDesc.vLifeTime = m_tParticleDesc.vLifeTime;
	ParticleDesc.fAge = m_tParticleDesc.fAge;
	ParticleDesc.vColor = m_tParticleDesc.vColor;
	//!ParticleDesc.vCenter = _float3(0.f, 0.f, 0.f);
	//!ParticleDesc.fRange = 3.f;
	//!ParticleDesc.vScale = _float2(0.2f, 0.5f);
	//!ParticleDesc.vSpeed = _float2(0.1f, 5.0f);
	//!ParticleDesc.vLifeTime = _float2(0.5f, 3.0f);

	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_VIBuffer_Particle_Point"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &ParticleDesc)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, m_tParticleDesc.strTextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Object::Bind_ShaderResources()
{	
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CParticle_Object * CParticle_Object::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eLevel)
{
	CParticle_Object*		pInstance = new CParticle_Object(pDevice, pContext);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CParticle_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CParticle_Object::Clone(void* pArg)
{
	CParticle_Object*		pInstance = new CParticle_Object(*this);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CParticle_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CParticle_Object::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}

