#include "stdafx.h"
#include "BackGround_Loading.h"
#include "GameInstance.h"


CBackGround_Loading::CBackGround_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice,pContext)
{
	
}

CBackGround_Loading::CBackGround_Loading(const CBackGround_Loading& rhs)
	:CGameObject(rhs)
{
}

HRESULT CBackGround_Loading::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CBackGround_Loading::Initialize(void* pArg)
{
	BACKGROUND_DESC*	pDesc = (BACKGROUND_DESC*)pArg;

	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;

	//TODO TransformCom 객체는 월드상에서 보여줄 게임 객체들은 전부 가지고있어야한다. 거의 대다수의 경우가 필요하기에
	//TODO 부모객체인 GameObject에서 만들어 줄 것이다. 그래서 부모객체의 Initialize 함수를 호출한다
	if(FAILED(__super::Initialize(pArg))) 
		return E_FAIL;

	if(FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaling(m_fSizeX, m_fSizeY, 1.f);
	
	m_pTransformCom->Set_State
	(
		CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.1f, 1.f)
	);
	
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));


	CBackGround_LoadingUI::BACKGROUND_DESC	BackGroundDesc = {};

	BackGroundDesc.fX = g_iWinSizeX >> 1;
	BackGroundDesc.fY = g_iWinSizeY >> 1;
	BackGroundDesc.fSizeX = 1050;
	BackGroundDesc.fSizeY = 150;
	

	CGameObject* pGameObject = { nullptr };

 	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_LOADING, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_BackGround_LoadingUI"), &BackGroundDesc, &pGameObject)))
		return E_FAIL;

	

	return S_OK;
}

void CBackGround_Loading::Priority_Tick(_float fTimeDelta)
{
	int i = 0;
}

void CBackGround_Loading::Tick(_float fTimeDelta)
{
	int i = 0;
	m_fFrame += 6.f * fTimeDelta;
	
	if (m_fFrame >= 6.0f)
		m_fFrame = 0.f;

	
}

void CBackGround_Loading::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this)))
		return;
}

HRESULT CBackGround_Loading::Render()
{
	//TODO 셰이더에게 행렬을 던져주는 행위는 반드시 셰이더의 비긴함수를 호출하기 이전에 해야한다.
	//! 그 이유는, 셰이더의 비긴함수 내에서 pPass->Apply(0, m_pContext); 코드를 수행한다.
	//! Apply 호출 후에 행렬을 던져줘도 에러는 나지 않지만, 안정성이 떨어진다.
	//! Apply 호출 후에 행렬을 던져주면, 어떤 때에는 정상적으로 수행되고, 어떤 때에는 값이 제대로 안 넘어가는 경우가 있다.
	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//! 이 셰이더에 0번째 패스로 그릴거야.
	m_pShaderCom->Begin(0);

	//! 내가 그리려고 하는 정점, 인덱스 버퍼를 장치에 바인딩해
	m_pVIBufferCom->Bind_VIBuffers();

	//! 바인딩된 정점, 인덱스를 그려
	m_pVIBufferCom->Render();
	
	return S_OK;
}

HRESULT CBackGround_Loading::Ready_Components()
{
	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	
	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	//! For.Com_Texture
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	////! For.Com_Texture
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LoadingTexture"),
	//	TEXT("Com_LoadingTexture"), reinterpret_cast<CComponent**>(&m_pLoadingTextureCom))))
	//	return E_FAIL;

	

	return S_OK;
}

HRESULT CBackGround_Loading::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if(FAILED(m_pTextureCom->Bind_ShaderResourceArray(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	//if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_LoadingTexture", _uint(m_fFrame))))
	//	return E_FAIL;
	
	return S_OK;
}

CBackGround_Loading* CBackGround_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBackGround_Loading* pInstance = new CBackGround_Loading(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround_Loading");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBackGround_Loading::Clone(void* pArg)
{
	CBackGround_Loading* pInstance = new CBackGround_Loading(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBackGround_Loading");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBackGround_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);

	
}
