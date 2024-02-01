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
	//TODO ������ü�� �ʱ�ȭ������ �����Ѵ�.
	/* 1.�����κ��� ���� �޾ƿͼ� �ʱ�ȭ�Ѵ� .*/
	/* 2.����������� ���� ���� �޾ƿ´�.*/

	return S_OK;
}

HRESULT CBackGround_Loading::Initialize(void* pArg)
{
	BACKGROUND_DESC*	pDesc = (BACKGROUND_DESC*)pArg;

	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;

	//TODO TransformCom ��ü�� ����󿡼� ������ ���� ��ü���� ���� �������־���Ѵ�. ���� ��ټ��� ��찡 �ʿ��ϱ⿡
	//TODO �θ�ü�� GameObject���� ����� �� ���̴�. �׷��� �θ�ü�� Initialize �Լ��� ȣ���Ѵ�
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
	//TODO ���̴����� ����� �����ִ� ������ �ݵ�� ���̴��� ����Լ��� ȣ���ϱ� ������ �ؾ��Ѵ�.
	//! �� ������, ���̴��� ����Լ� ������ pPass->Apply(0, m_pContext); �ڵ带 �����Ѵ�.
	//! Apply ȣ�� �Ŀ� ����� �����൵ ������ ���� ������, �������� ��������.
	//! Apply ȣ�� �Ŀ� ����� �����ָ�, � ������ ���������� ����ǰ�, � ������ ���� ����� �� �Ѿ�� ��찡 �ִ�.
	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//! �� ���̴��� 0��° �н��� �׸��ž�.
	m_pShaderCom->Begin(0);

	//! ���� �׸����� �ϴ� ����, �ε��� ���۸� ��ġ�� ���ε���
	m_pVIBufferCom->Bind_VIBuffers();

	//! ���ε��� ����, �ε����� �׷�
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

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
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

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
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
