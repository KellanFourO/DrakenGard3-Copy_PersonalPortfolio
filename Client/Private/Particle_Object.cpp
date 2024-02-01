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
	/* 원형객체의 초기화과정을 수행한다. */
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/
	//int*		pData_int = new int(3);

	//void**		pData = (void**)&pData_int;

	return S_OK;
}

HRESULT CParticle_Object::Initialize(void* pArg)
{	
	m_tParticleDesc = *(PARTICLE_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	

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
	

	//if(true == m_bParticleStart)
	 m_pVIBufferCom->Update(fTimeDelta);


	 if (true == m_tParticleDesc.bMultiSpriteAnim)
	 {
		m_fFrame += fTimeDelta * m_tParticleDesc.fSpriteSpeed;

		if (m_fFrame >= m_tParticleDesc.fMultiSpriteCount)
			m_fFrame = 0.f;
	 }

	 

	 m_fTimeAcc += fTimeDelta;

	 if (m_fTimeAcc > m_fAddTime)
	 {
		 m_iCurrentHor++;

		 if (m_iCurrentHor == m_iMaxHor)
		 {
			 m_iCurrentVer++;
			 m_iCurrentHor = m_iStartHor;

			 if (m_iCurrentVer == m_iMaxVer)
			 {
				 m_iCurrentVer = m_iStartVer;
			 }
		 }

		 m_fTimeAcc = 0.f;
	 }

	 m_fAge += fTimeDelta;

	 if (m_fAge >= m_tParticleDesc.vLifeTime.y)
	 {
		Set_Dead();
		m_fAge = 0.f;
	 }


}

void CParticle_Object::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this)))
		return;
}

HRESULT CParticle_Object::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 셰ㅒ이더에 0번째 패스로 그릴꺼야. */
	m_pShaderCom->Begin(m_tParticleDesc.iShaderPathIndex);

	/* 내가 그릴려고하는 정점, 인덷ㄱ스버퍼를 장치에 바인딩해. */
	m_pVIBufferCom->Bind_VIBuffers();

	/* 바인딩된 정점, 인덱스를 그려. */
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
	ParticleDesc.iNumInstance = m_tParticleDesc.iNumInstance;
	ParticleDesc.vCenter = m_tParticleDesc.vCenter;
	ParticleDesc.fRange = m_tParticleDesc.fRange;
	ParticleDesc.vScale = m_tParticleDesc.vScale;
	ParticleDesc.vSpeed = m_tParticleDesc.vSpeed;
	ParticleDesc.vRotation = m_tParticleDesc.vRotation;
	ParticleDesc.vLifeTime = m_tParticleDesc.vLifeTime;
	ParticleDesc.fAge = m_tParticleDesc.fAge;
	ParticleDesc.vColor = m_tParticleDesc.vColor;
	ParticleDesc.bRandom = m_tParticleDesc.bRandom;
	ParticleDesc.vDir = m_tParticleDesc.vDir;
	ParticleDesc.vRandomRotation = m_tParticleDesc.vRandomRotation;
	ParticleDesc.vInterval = m_tParticleDesc.vInterval;

	ParticleDesc.bTimeScale = m_tParticleDesc.bTimeScale;
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

	if (true == m_tParticleDesc.bMultiSpriteAnim)
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", _uint(m_fFrame))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
			return E_FAIL;
	}

	

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	_float2 uvOffset = { (_float)(m_iCurrentHor * m_fAnimationSizeX) / m_fSpriteSizeX, (_float)(m_iCurrentVer * m_fAnimationSizeY) / m_fSpriteSizeY };
	_float2 uvScale = { (_float)m_fAnimationSizeX / m_fSpriteSizeX, (_float)m_fAnimationSizeY / m_fSpriteSizeY };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVOffset", &uvOffset, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &uvScale, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

CParticle_Object * CParticle_Object::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eLevel)
{
	CParticle_Object*		pInstance = new CParticle_Object(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
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

	/* 원형객체를 초기화한다.  */
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

