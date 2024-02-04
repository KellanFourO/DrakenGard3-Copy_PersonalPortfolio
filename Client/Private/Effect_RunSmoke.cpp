#include "stdafx.h"
#include "..\Public\Effect_RunSmoke.h"

#include "GameInstance.h"


CEffect_RunSmoke::CEffect_RunSmoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAlphaObject(pDevice, pContext)
{

}

CEffect_RunSmoke::CEffect_RunSmoke(const CEffect_RunSmoke & rhs)
	: CAlphaObject(rhs)
{
}

HRESULT CEffect_RunSmoke::Initialize_Prototype()
{
	/* 원형객체의 초기화과정을 수행한다. */
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/
	//int*		pData_int = new int(3);

	//void**		pData = (void**)&pData_int;

	return S_OK;
}

HRESULT CEffect_RunSmoke::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// void* pArg : 복제하여 객체를 생성할때 원형에서 채우지 못한 데이터를 추가적으로 더 초기화해주기위해서. 
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_tEffectDesc = *(EFFECT_DESC*)pArg;
	
	m_iCurrentHor = m_tEffectDesc.iStartHor;
	m_iCurrentVer = m_tEffectDesc.iStartVer;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_tEffectDesc.vCreatePos));
	m_pTransformCom->Look_At_Dir(m_tEffectDesc.vDir);
	m_pTransformCom->Set_Scaling(m_tEffectDesc.vScale.x, m_tEffectDesc.vScale.y, m_tEffectDesc.vScale.z);


	m_fLifeTime = m_tEffectDesc.fLifeTime;
	return S_OK;
}

void CEffect_RunSmoke::Priority_Tick(_float fTimeDelta)
{
	int a = 10;

	
}

void CEffect_RunSmoke::Tick(_float fTimeDelta)
{
	//m_fFrame += 7.f * fTimeDelta * m_tEffectDesc.fPlaySpeed;
	//
	//if (m_fFrame >= 7.0f)
	//	m_fFrame = 0.f;
	_float fTimeDelta_Shader = fTimeDelta;

	m_pShaderCom->Bind_RawValue("g_fTimeDelta", &fTimeDelta_Shader, sizeof(_float));
		

	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc > m_tEffectDesc.fPlaySpeed)
	{
		m_iCurrentHor++;

		if (m_iCurrentHor == m_tEffectDesc.iMaxHor)
		{
			m_iCurrentVer++;
			m_iCurrentHor = m_tEffectDesc.iStartHor;

			if (m_iCurrentVer == m_tEffectDesc.iMaxVer)
			{
				m_iCurrentVer = m_tEffectDesc.iStartVer;
				Die(0.1f);
			}
		}

		m_fTimeAcc = 0.f;
	}

	m_pRigidBodyCom->Tick(fTimeDelta);
}

void CEffect_RunSmoke::Late_Tick(_float fTimeDelta)
{
	Compute_CamDistance();


	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this)))
		return ;
}

HRESULT CEffect_RunSmoke::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 셰ㅒ이더에 0번째 패스로 그릴꺼야. */
	m_pShaderCom->Begin(9);

	/* 내가 그릴려고하는 정점, 인덷ㄱ스버퍼를 장치에 바인딩해. */
	m_pVIBufferCom->Bind_VIBuffers();

	/* 바인딩된 정점, 인덱스를 그려. */
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CEffect_RunSmoke::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RunSmoke"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	CRigidBody::RIGIDBODY_TYPE eType = CRigidBody::RIGIDBODY_TYPE::DYNAMIC;

	////TODO 리지드바디
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &eType)))
		return E_FAIL;

	m_pRigidBodyCom->Set_Owner(this);
	
	m_pRigidBodyCom->Clear_NetPower();
	m_pRigidBodyCom->Set_UseGravity(true);

	return S_OK;
}

HRESULT CEffect_RunSmoke::Bind_ShaderResources()
{
	
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	//if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", _uint(m_fFrame))))
	//	return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	_float2 uvOffset = { (_float)(m_iCurrentHor * m_tEffectDesc.iAnimationSizeX) / m_tEffectDesc.iSpriteSizeX, (_float)(m_iCurrentVer * m_tEffectDesc.iAnimationSizeY) / m_tEffectDesc.iSpriteSizeY };
	_float2 uvScale = { (_float)m_tEffectDesc.iAnimationSizeX/ m_tEffectDesc.iSpriteSizeX, (_float)m_tEffectDesc.iAnimationSizeY/ m_tEffectDesc.iSpriteSizeY};

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVOffset", &uvOffset, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &uvScale, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamDirection", &m_pGameInstance->Get_CamDir(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
		return E_FAIL;

	return S_OK;
}

CEffect_RunSmoke * CEffect_RunSmoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffect_RunSmoke*		pInstance = new CEffect_RunSmoke(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_RunSmoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_RunSmoke::Clone(void* pArg)
{
	CEffect_RunSmoke*		pInstance = new CEffect_RunSmoke(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_RunSmoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_RunSmoke::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}

