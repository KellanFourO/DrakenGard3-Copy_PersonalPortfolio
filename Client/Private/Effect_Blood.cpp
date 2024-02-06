#include "stdafx.h"
#include "..\Public\Effect_Blood.h"

#include "GameInstance.h"


CEffect_Blood::CEffect_Blood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAlphaObject(pDevice, pContext)
{

}

CEffect_Blood::CEffect_Blood(const CEffect_Blood & rhs)
	: CAlphaObject(rhs)
{
}

HRESULT CEffect_Blood::Initialize_Prototype()
{
	/* 원형객체의 초기화과정을 수행한다. */
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/
	//int*		pData_int = new int(3);

	//void**		pData = (void**)&pData_int;

	return S_OK;
}

HRESULT CEffect_Blood::Initialize(void* pArg)
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

	_int iRandomNumber = Random({1,2,3,6,7});

	wstring strSoundName = L"Hit";
	strSoundName += to_wstring(iRandomNumber);
	strSoundName += L".wav";

	m_pGameInstance->Play_Sound(L"HIT_EFFECT", strSoundName, SOUND_EFFECT3, 0.75f);

	return S_OK;
}

void CEffect_Blood::Priority_Tick(_float fTimeDelta)
{
	int a = 10;

	
}

void CEffect_Blood::Tick(_float fTimeDelta)
{
	//m_fFrame += 7.f * fTimeDelta * m_tEffectDesc.fPlaySpeed;
	//
	//if (m_fFrame >= 7.0f)
	//	m_fFrame = 0.f;

	

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
				m_pGameInstance->Stop_Sound(SOUND_EFFECT3);
				No_DissoveDie(0.1f);
			}
		}

		m_fTimeAcc = 0.f;
	}

	m_pRigidBodyCom->Tick(fTimeDelta);

	
}

void CEffect_Blood::Late_Tick(_float fTimeDelta)
{
	Compute_CamDistance();

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this)))
		return ;
}

HRESULT CEffect_Blood::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 셰ㅒ이더에 0번째 패스로 그릴꺼야. */
	m_pShaderCom->Begin(5);

	/* 내가 그릴려고하는 정점, 인덷ㄱ스버퍼를 장치에 바인딩해. */
	m_pVIBufferCom->Bind_VIBuffers();

	/* 바인딩된 정점, 인덱스를 그려. */
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CEffect_Blood::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Blood"),
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

HRESULT CEffect_Blood::Bind_ShaderResources()
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

CEffect_Blood * CEffect_Blood::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffect_Blood*		pInstance = new CEffect_Blood(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Blood");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Blood::Clone(void* pArg)
{
	CEffect_Blood*		pInstance = new CEffect_Blood(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Blood");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Blood::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}

