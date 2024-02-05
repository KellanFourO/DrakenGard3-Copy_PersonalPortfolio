#include "stdafx.h"
#include "UI_Blood.h"
#include "GameInstance.h"

CUI_Blood::CUI_Blood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eCurrentLevel)
	:CMyUI(pDevice,pContext)
	, m_eCurrentLevel(eCurrentLevel)
{
	
}

CUI_Blood::CUI_Blood(const CUI_Blood& rhs)
	: CMyUI(rhs)
	, m_eCurrentLevel(rhs.m_eCurrentLevel)
{
}

HRESULT CUI_Blood::Initialize_Prototype()
{
	//TODO ������ü�� �ʱ�ȭ������ �����Ѵ�.
	/* 1.�����κ��� ���� �޾ƿͼ� �ʱ�ȭ�Ѵ� .*/
	/* 2.����������� ���� ���� �޾ƿ´�.*/

	return S_OK;
}

HRESULT CUI_Blood::Initialize(void* pArg)
{
	UI_DESC Desc = *(UI_DESC*)pArg;

	m_RandomNumber = mt19937_64(m_RandomDevice());

	uniform_real_distribution<float>	RandomPosX(0, g_iWinSizeX);
	uniform_real_distribution<float>	RandomPosY(0, g_iWinSizeY);

	uniform_real_distribution<float>	RandomSizeX(0, 100.f);
	uniform_real_distribution<float>	RandomSizeY(0, 100.f);
		
		
		
	Desc.fX = RandomPosX(m_RandomNumber);
	Desc.fY = RandomPosY(m_RandomNumber);
	Desc.fSizeX = RandomSizeX(m_RandomNumber);
	Desc.fSizeY = Desc.fSizeX;
	


	if (FAILED(Ready_Components()))
		return E_FAIL;

	if(FAILED(__super::Initialize(&Desc))) //!  Ʈ������ ����, m_tInfo�� bWorldUI �� false �� ��쿡�� ������ġ ����
		return E_FAIL;


	m_iRandomNumber = Random({1, 2, 3, 4, 5, 6, 7});

	return S_OK;
}

void CUI_Blood::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Blood::Tick(_float fTimeDelta)
{
	
}

void CUI_Blood::Late_Tick(_float fTimeDelta)
{

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CUI_Blood::Render()
{
	//TODO ���̴����� ����� �����ִ� ������ �ݵ�� ���̴��� ����Լ��� ȣ���ϱ� ������ �ؾ��Ѵ�.
	//! �� ������, ���̴��� ����Լ� ������ pPass->Apply(0, m_prContext); �ڵ带 �����Ѵ�.
	//! Apply ȣ�� �Ŀ� ����� �����൵ ������ ���� ������, �������� ��������.
	//! Apply ȣ�� �Ŀ� ����� �����ָ�, � ������ ���������� ����ǰ�, � ������ ���� ����� �� �Ѿ�� ��찡 �ִ�.
	
	if(false == m_isEnable)
		return E_FAIL;
	
	_bool bCustomColor = true;
	m_pShaderCom->Bind_RawValue("g_bCustomColor", &bCustomColor, sizeof(_bool));
	_float4 vColor = _float4(0.7f, 0.0f, 0.1f, 1.0f);
	m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));
	
		
	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//! �� ���̴��� 0��° �н��� �׸��ž�.
	m_pShaderCom->Begin(12); //! Shader_PosTex 7�� �н� = VS_MAIN,  PS_UI_HP

	//! ���� �׸����� �ϴ� ����, �ε��� ���۸� ��ġ�� ���ε���
	m_pVIBufferCom->Bind_VIBuffers();

	//! ���ε��� ����, �ε����� �׷�
	m_pVIBufferCom->Render();

	bCustomColor = false;

	m_pShaderCom->Bind_RawValue("g_bCustomColor", &bCustomColor, sizeof(_bool));
	return S_OK;
}

HRESULT CUI_Blood::Ready_Components()
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
	if (FAILED(__super::Add_Component(m_eCurrentLevel, TEXT("Prototype_Component_Texture_UI_Blood"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	//! For.Com_Texture
	if (FAILED(__super::Add_Component(m_eCurrentLevel, TEXT("Prototype_Component_Texture_UI_BloodMask"),
		TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTexture))))
		return E_FAIL;

	//! For.Com_Texture
	if (FAILED(__super::Add_Component(m_eCurrentLevel, TEXT("Prototype_Component_Texture_UI_BloodNoise"),
		TEXT("Com_NoiseTexture"), reinterpret_cast<CComponent**>(&m_pNoiseTexture))))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CUI_Blood::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iRandomNumber)))
		return E_FAIL;

	if (FAILED(m_pMaskTexture->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;

	if (FAILED(m_pNoiseTexture->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture")))
		return E_FAIL;
	
	return S_OK;
}


CUI_Blood* CUI_Blood::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eCurrentLevel)
{
	CUI_Blood* pInstance = new CUI_Blood(pDevice, pContext, eCurrentLevel);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Blood");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Blood::Clone(void* pArg)
{
	CUI_Blood* pInstance = new CUI_Blood(*this);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Blood");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Blood::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMaskTexture);
	Safe_Release(m_pNoiseTexture);

	
}
