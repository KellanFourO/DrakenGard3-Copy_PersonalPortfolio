#include "stdafx.h"
#include "..\Public\Effect_BornFire.h"

#include "GameInstance.h"


CEffect_BornFire::CEffect_BornFire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAlphaObject(pDevice, pContext)
{

}

CEffect_BornFire::CEffect_BornFire(const CEffect_BornFire & rhs)
	: CAlphaObject(rhs)
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

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(10 + rand() % 20, 19.f, rand() % 20, 1.f));	

	return S_OK;
}

void CEffect_BornFire::Priority_Tick(_float fTimeDelta)
{
	int a = 10;

	
}

void CEffect_BornFire::Tick(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc > m_fAddTime)
	{
		m_iVerFrame++;

		if (m_iVerFrame == m_iMaxVer)
		{
			m_iHorFrame++;
			m_iVerFrame = 0;

			if (m_iHorFrame == m_iMaxHor)
			{
				m_iHorFrame = 0;
			}
		}

		m_fTimeAcc = 0.f;
	}
}

void CEffect_BornFire::Late_Tick(_float fTimeDelta)
{
	Compute_CamDistance();

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this)))
		return ;
}

HRESULT CEffect_BornFire::Render()
{
	

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 셰ㅒ이더에 0번째 패스로 그릴꺼야. */
	m_pShaderCom->Begin(3);

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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BornFire"),
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

	_float SizeX = m_fWidthSize * m_iNumVer;
	_float SizeY = m_fHeightSize * m_iNumHor;

	if(FAILED(m_pShaderCom->Bind_RawValue("g_fAtlasSizeX", &SizeX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAtlasSizeY", &SizeY, sizeof(_float))))
		return E_FAIL;

	

	_float u2 = (m_fWidthSize * m_iVerFrame) / 2048.f;
	_float v2 = (m_fHeightSize * m_iHorFrame) / 2048.f;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAtlasPosX", &u2, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAtlasPosY", &v2, sizeof(_float))))
		return E_FAIL;



	//float			g_fAtlasPosX;
	//float			g_fAtlasPosY;
	//float			g_fAtlasSizeX;
	//float			g_fAtlasSizeY;

	if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
		return E_FAIL;

	return S_OK;
}

CEffect_BornFire * CEffect_BornFire::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffect_BornFire*		pInstance = new CEffect_BornFire(pDevice, pContext);

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

