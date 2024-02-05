#include "stdafx.h"
#include "..\Public\Effect_BornFire.h"

#include "GameInstance.h"


CEffect_BornFire::CEffect_BornFire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eCurrentLevel)
	: CAlphaObject(pDevice, pContext)
	,m_eCurrentLevel(eCurrentLevel)
{

}

CEffect_BornFire::CEffect_BornFire(const CEffect_BornFire & rhs)
	: CAlphaObject(rhs)
	,m_eCurrentLevel(rhs.m_eCurrentLevel)
{
}

HRESULT CEffect_BornFire::Initialize_Prototype()
{
	/* 원형객체의 초기화과정을 수행한다. */
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/
	//int*		pData_int = new int(3);

	//void**		pData = (void**)&pData_int;

	return S_OK;
}

HRESULT CEffect_BornFire::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// void* pArg : 복제하여 객체를 생성할때 원형에서 채우지 못한 데이터를 추가적으로 더 초기화해주기위해서. 
	if (FAILED(Ready_Components()))
		return E_FAIL;

	BORNFIRE_DESC Desc = *(BORNFIRE_DESC*)pArg;

	_vector vCreatePos = XMLoadFloat4(&Desc.vPos);
	vCreatePos.m128_f32[3] = 1.f;

	m_pTransformCom->Set_Scaling(Desc.vScale.x, Desc.vScale.y, Desc.vScale.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Desc.vPos);
	m_pTarget = Desc.pTarget;
	m_vCreatePos = Desc.vPos;
	m_vCreateLook = Desc.vLook;
	m_fLifeTime = Desc.fLifeTime;
	m_bBreath = Desc.bBossBreath;
	//_vector vPlayerPos = m_pGameInstance->Get_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(10 + rand() % 20, 5.f, rand() % 20, 1.f));	
	//vPlayerPos.m128_f32[1] += 1.f;
	m_pTransformCom->Look_At_Dir(XMLoadFloat4(&m_vCreateLook));

	return S_OK;
}

void CEffect_BornFire::Priority_Tick(_float fTimeDelta)
{
	int a = 10;

	
}

void CEffect_BornFire::Tick(_float fTimeDelta)
{
	
	// 플레이어의 위치 가져오기
	//_vector vPlayerPos = m_pGameInstance->Get_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	//
	//// 객체가 바라보는 방향으로 회전
	//m_pTransformCom->Look_At(vPlayerPos);

	if (true == m_bBreath)
	{
		m_fAge += fTimeDelta;
		
		

		if(m_fAge >= m_fLifeTime)
			Set_Dead();

		m_pTransformCom->Go_Straight(fTimeDelta);
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

	

	
}

void CEffect_BornFire::Late_Tick(_float fTimeDelta)
{
	Compute_CamDistance();

	if (m_fCamDistance < 6.f && false == m_bPlaySound)
	{
		m_pGameInstance->Play_Sound(L"ENVIRONMENT_EFFECT", L"Burn.wav", SOUND_ENVIRONMENT, 0.75f);
		m_bPlaySound = true;
	}
	else if (m_fCamDistance > 6.f && true == m_bPlaySound)
	{
		m_pGameInstance->Stop_Sound(SOUND_ENVIRONMENT);
		m_bPlaySound = false;
	}

	
		

	

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this)))
		return ;
}

HRESULT CEffect_BornFire::Render()
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

HRESULT CEffect_BornFire::Ready_Components()
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
	if (FAILED(__super::Add_Component(m_eCurrentLevel, TEXT("Prototype_Component_Texture_BornFire"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;



	return S_OK;
}

HRESULT CEffect_BornFire::Bind_ShaderResources()
{
	
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamDirection", &m_pGameInstance->Get_CamDir(), sizeof(_float4))))
		return E_FAIL;

	_float2 uvOffset = {(_float)(m_iCurrentHor * m_fAnimationSizeX) / m_fSpriteSizeX, (_float)(m_iCurrentVer * m_fAnimationSizeY) / m_fSpriteSizeY };
	_float2 uvScale =  { (_float)m_fAnimationSizeX / m_fSpriteSizeX, (_float)m_fAnimationSizeY / m_fSpriteSizeY};
	
	if(FAILED(m_pShaderCom->Bind_RawValue("g_UVOffset", &uvOffset, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &uvScale, sizeof(_float2))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
		return E_FAIL;

	return S_OK;
}



CEffect_BornFire * CEffect_BornFire::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eCurrentLevel)
{
	CEffect_BornFire*		pInstance = new CEffect_BornFire(pDevice, pContext, eCurrentLevel);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_BornFire");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_BornFire::Clone(void* pArg)
{
	CEffect_BornFire*		pInstance = new CEffect_BornFire(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_BornFire");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_BornFire::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}

