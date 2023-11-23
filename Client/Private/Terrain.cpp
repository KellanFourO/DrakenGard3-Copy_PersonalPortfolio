#include "stdafx.h"
#include "Terrain.h"
#include "GameInstance.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice,pContext)
{
	
}

CTerrain::CTerrain(const CTerrain& rhs)
	:CGameObject(rhs)
	, m_eCurrentLevelID(rhs.m_eCurrentLevelID)
{
}

HRESULT CTerrain::Initialize_Prototype(LEVEL eLevel)
{
	//TODO ������ü�� �ʱ�ȭ������ �����Ѵ�.
	/* 1.�����κ��� ���� �޾ƿͼ� �ʱ�ȭ�Ѵ� .*/
	/* 2.����������� ���� ���� �޾ƿ´�.*/

	m_eCurrentLevelID = eLevel;

	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{

	if(FAILED(__super::Initialize(pArg))) 
		return E_FAIL;


	if(FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CTerrain::Priority_Tick(_float fTimeDelta)
{
	int i = 0;
}

void CTerrain::Tick(_float fTimeDelta)
{
	int i = 0;

	if (GetKeyState(VK_LEFT) & 0x8000)
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
	if (GetKeyState(VK_RIGHT) & 0x8000)
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);

	if (GetKeyState(VK_UP) & 0x8000)
		m_pTransformCom->Go_Straight(fTimeDelta);
	if (GetKeyState(VK_DOWN) & 0x8000)
		m_pTransformCom->Go_Backward(fTimeDelta);
}

void CTerrain::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CTerrain::Render()
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

HRESULT CTerrain::Ready_Components()
{
	//! For.Com_Shader
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	
	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	//! For.Com_Texture
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
{

	//TODO ī�޷� ��� �ӽ÷� ��Ƴ����ž�.
	_float4x4		ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 30.f, -20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), g_iWinSizeX / (_float)g_iWinSizeY, 0.2f, 1000.f));

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResourceArray(m_pShaderCom, "g_Texture")))
		return E_FAIL;
	
	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	
}
