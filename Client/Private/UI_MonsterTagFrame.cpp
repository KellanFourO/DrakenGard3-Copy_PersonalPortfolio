#include "stdafx.h"
#include "UI_MonsterTagFrame.h"
#include "GameInstance.h"

CUI_MonsterTagFrame::CUI_MonsterTagFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eCurrentLevel)
	:CMyUI(pDevice,pContext)
	, m_eCurrentLevel(eCurrentLevel)
{
	
}

CUI_MonsterTagFrame::CUI_MonsterTagFrame(const CUI_MonsterTagFrame& rhs)
	: CMyUI(rhs)
	, m_eCurrentLevel(rhs.m_eCurrentLevel)
{
}

HRESULT CUI_MonsterTagFrame::Initialize_Prototype()
{
	//TODO ������ü�� �ʱ�ȭ������ �����Ѵ�.
	/* 1.�����κ��� ���� �޾ƿͼ� �ʱ�ȭ�Ѵ� .*/
	/* 2.����������� ���� ���� �޾ƿ´�.*/

	return S_OK;
}

HRESULT CUI_MonsterTagFrame::Initialize(void* pArg)
{
	m_tInfo = *(MONSTER_HP*)pArg;
	m_tInfo.bFrame = true;
	m_isEnable = m_tInfo.bEnable;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if(FAILED(__super::Initialize(pArg))) //!  Ʈ������ ����, m_tInfo�� bWorldUI �� false �� ��쿡�� ������ġ ����
		return E_FAIL;

	m_fCurrentHp = m_tInfo.pOwnerStatus->fCurrentHp;
	m_fPrevHp = m_fCurrentHp;
	m_tInfo.fCrntHPUV = m_tInfo.pOwnerStatus->fCurrentHp / m_tInfo.pOwnerStatus->fMaxHp;
	SetUp_UV(0);
	

	return S_OK;
}

void CUI_MonsterTagFrame::Priority_Tick(_float fTimeDelta)
{
}

void CUI_MonsterTagFrame::Tick(_float fTimeDelta)
{


	m_tInfo.fCrntHPUV = m_tInfo.pOwnerStatus->fCurrentHp / m_tInfo.pOwnerStatus->fMaxHp; 
	
	if (m_tInfo.fCrntHPUV <= 0.0)
	{
		m_tInfo.fCrntHPUV = 0.0;

		Set_Dead();
	}
	
	
}

void CUI_MonsterTagFrame::Late_Tick(_float fTimeDelta)
{
	Compute_OwnerCamDistance();

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CUI_MonsterTagFrame::Render()
{
	//TODO ���̴����� ����� �����ִ� ������ �ݵ�� ���̴��� ����Լ��� ȣ���ϱ� ������ �ؾ��Ѵ�.
	//! �� ������, ���̴��� ����Լ� ������ pPass->Apply(0, m_prContext); �ڵ带 �����Ѵ�.
	//! Apply ȣ�� �Ŀ� ����� �����൵ ������ ���� ������, �������� ��������.
	//! Apply ȣ�� �Ŀ� ����� �����ָ�, � ������ ���������� ����ǰ�, � ������ ���� ����� �� �Ѿ�� ��찡 �ִ�.
	
	if(false == m_isEnable)
		return E_FAIL;
	
	
	if (m_tInfo.eMonsterType == CUI_MonsterTagFrame::BOSS)
	{

	}
	else if (m_tInfo.eMonsterType == CUI_MonsterTagFrame::COMMON)
	{
		if (m_fOwnerCamDistance > 40.f || false == In_Frustum())
		{
			m_pGameInstance->Get_CamDir();
			return E_FAIL;
		}
		__super::SetUp_WorldToScreen(m_tInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION));

		__super::SetUp_Transform(m_fWorldToScreenX + 10.f, m_fWorldToScreenY - 15.f, m_fDefaultScale * m_fScaleOffsetX, m_fDefaultScale * m_fScaleOffsetY);


		//__super::SetUp_BillBoarding();
	}

	if(FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//! �� ���̴��� 0��° �н��� �׸��ž�.
	m_pShaderCom->Begin(8); //! Shader_PosTex 7�� �н� = VS_MAIN,  PS_UI_HP

	//! ���� �׸����� �ϴ� ����, �ε��� ���۸� ��ġ�� ���ε���
	m_pVIBufferCom->Bind_VIBuffers();

	//! ���ε��� ����, �ε����� �׷�
	m_pVIBufferCom->Render();
	
	return S_OK;
}

HRESULT CUI_MonsterTagFrame::Ready_Components()
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
	if (FAILED(__super::Add_Component(m_eCurrentLevel, TEXT("Prototype_Component_Texture_EnemyTagFrame"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	

	

	return S_OK;
}

HRESULT CUI_MonsterTagFrame::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;
	
	return S_OK;
}

void CUI_MonsterTagFrame::Compute_OwnerCamDistance()
{
	_vector		vPosition = m_tInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION);
	_vector		vCamPosition = XMLoadFloat4(&m_pGameInstance->Get_CamPosition());

	m_fOwnerCamDistance = XMVectorGetX(XMVector3Length(vPosition - vCamPosition));
}

_bool CUI_MonsterTagFrame::In_Frustum()
{
	return m_pGameInstance->isIn_WorldPlanes(m_tInfo.pOwnerTransform->Get_State(CTransform::STATE_POSITION), 2.f);
}

CUI_MonsterTagFrame* CUI_MonsterTagFrame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eCurrentLevel)
{
	CUI_MonsterTagFrame* pInstance = new CUI_MonsterTagFrame(pDevice, pContext, eCurrentLevel);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_MonsterTagFrame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_MonsterTagFrame::Clone(void* pArg)
{
	CUI_MonsterTagFrame* pInstance = new CUI_MonsterTagFrame(*this);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_MonsterTagFrame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_MonsterTagFrame::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	
}
