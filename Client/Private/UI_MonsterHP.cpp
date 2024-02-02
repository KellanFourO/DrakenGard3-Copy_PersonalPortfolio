#include "stdafx.h"
#include "UI_MonsterHP.h"
#include "GameInstance.h"

CUI_MonsterHP::CUI_MonsterHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eCurrentLevel)
	:CMyUI(pDevice,pContext)
	, m_eCurrentLevel(eCurrentLevel)
{
	
}

CUI_MonsterHP::CUI_MonsterHP(const CUI_MonsterHP& rhs)
	: CMyUI(rhs)
	, m_eCurrentLevel(rhs.m_eCurrentLevel)
{
}

HRESULT CUI_MonsterHP::Initialize_Prototype()
{
	//TODO ������ü�� �ʱ�ȭ������ �����Ѵ�.
	/* 1.�����κ��� ���� �޾ƿͼ� �ʱ�ȭ�Ѵ� .*/
	/* 2.����������� ���� ���� �޾ƿ´�.*/

	return S_OK;
}

HRESULT CUI_MonsterHP::Initialize(void* pArg)
{
	m_tInfo = *(MONSTER_HP*)pArg;
	m_tInfo.bFrame = false;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if(FAILED(__super::Initialize(pArg))) //!  Ʈ������ ����, m_tInfo�� bWorldUI �� false �� ��쿡�� ������ġ ����
		return E_FAIL;

	

	m_fCurrentHp = m_tInfo.pOwnerStatus->fCurrentHp;
	m_fPrevHp = m_fCurrentHp;
	m_tInfo.fCrntHPUV = m_tInfo.pOwnerStatus->fCurrentHp / m_tInfo.pOwnerStatus->fMaxHp;
	m_tInfo.fPrevHPUV = m_tInfo.fCrntHPUV;
	
	
	SetUp_UV(0);
	

	return S_OK;
}

void CUI_MonsterHP::Priority_Tick(_float fTimeDelta)
{
}

void CUI_MonsterHP::Tick(_float fTimeDelta)
{

	m_tInfo.fCrntHPUV = m_tInfo.pOwnerStatus->fCurrentHp / m_tInfo.pOwnerStatus->fMaxHp; 
	m_fCurrentHp = m_tInfo.pOwnerStatus->fCurrentHp;

	if (m_tInfo.fCrntHPUV < m_tInfo.fPrevHPUV)
	{
		m_tInfo.fPrevHPUV -= fTimeDelta * 0.3f;
	}

	if (m_tInfo.fPrevHPUV <= m_tInfo.fCrntHPUV)
	{
		m_tInfo.fPrevHPUV = m_tInfo.fCrntHPUV;
	}
	
	
	if (m_tInfo.fCrntHPUV <= 0.0)
	{
		m_tInfo.fCrntHPUV = 0.0;

		Set_Dead();
	}
	
	
}

void CUI_MonsterHP::Late_Tick(_float fTimeDelta)
{
	Compute_OwnerCamDistance();

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CUI_MonsterHP::Render()
{
	//TODO ���̴����� ����� �����ִ� ������ �ݵ�� ���̴��� ����Լ��� ȣ���ϱ� ������ �ؾ��Ѵ�.
	//! �� ������, ���̴��� ����Լ� ������ pPass->Apply(0, m_prContext); �ڵ带 �����Ѵ�.
	//! Apply ȣ�� �Ŀ� ����� �����൵ ������ ���� ������, �������� ��������.
	//! Apply ȣ�� �Ŀ� ����� �����ָ�, � ������ ���������� ����ǰ�, � ������ ���� ����� �� �Ѿ�� ��찡 �ִ�.
	//! 
	if (m_tInfo.eMonsterType == CUI_MonsterHP::BOSS)
	{

	}
	else if (m_tInfo.eMonsterType == CUI_MonsterHP::COMMON)
	{

		if (m_fOwnerCamDistance > 40.f)
		{
			m_pGameInstance->Get_CamDir();
			return E_FAIL;
		}
		__super::SetUp_WorldToScreen(m_tInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION));
		__super::SetUp_Transform(m_fWorldToScreenX, m_fWorldToScreenY, m_fDefaultScale * m_fScaleOffsetX, m_fDefaultScale * m_fScaleOffsetY);


		//__super::SetUp_BillBoarding();
	}

	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//! �� ���̴��� 0��° �н��� �׸��ž�.
	m_pShaderCom->Begin(7); //! Shader_PosTex 7�� �н� = VS_MAIN,  PS_UI_HP

	//! ���� �׸����� �ϴ� ����, �ε��� ���۸� ��ġ�� ���ε���
	m_pVIBufferCom->Bind_VIBuffers();

	//! ���ε��� ����, �ε����� �׷�
	m_pVIBufferCom->Render();
	
	return S_OK;
}

HRESULT CUI_MonsterHP::Ready_Components()
{
	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	
	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;


	if (m_tInfo.eMonsterType == CUI_MonsterHP::BOSS)
	{
		//! For.Com_Texture
		if (FAILED(__super::Add_Component(m_eCurrentLevel, TEXT("Prototype_Component_Texture_EnemyBossHP"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	else
	{
		//! For.Com_Texture
		if (FAILED(__super::Add_Component(m_eCurrentLevel, TEXT("Prototype_Component_Texture_EnemyHP"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CUI_MonsterHP::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCrntHPUV", &m_tInfo.fCrntHPUV, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fPrevHPUV", &m_tInfo.fPrevHPUV, sizeof(_float))))
		return E_FAIL;
	
	return S_OK;
}

void CUI_MonsterHP::Compute_OwnerCamDistance()
{
	_vector		vPosition = m_tInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION);
	_vector		vCamPosition = XMLoadFloat4(&m_pGameInstance->Get_CamPosition());

	m_fOwnerCamDistance = XMVectorGetX(XMVector3Length(vPosition - vCamPosition));
}

CUI_MonsterHP* CUI_MonsterHP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eCurrentLevel)
{
	CUI_MonsterHP* pInstance = new CUI_MonsterHP(pDevice, pContext, eCurrentLevel);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_MonsterHP");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_MonsterHP::Clone(void* pArg)
{
	CUI_MonsterHP* pInstance = new CUI_MonsterHP(*this);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_MonsterHP");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_MonsterHP::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	
}
