#include "stdafx.h"
#include "Camera_MapTool.h"
#include "GameInstance.h"

CCamera_MapTool::CCamera_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCamera(pDevice,pContext)
{
}

CCamera_MapTool::CCamera_MapTool(const CCamera_MapTool& rhs)
	:CCamera(rhs)
{
}

HRESULT CCamera_MapTool::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelID = eLevel;
	return S_OK;
}

HRESULT CCamera_MapTool::Initialize(void* pArg)
{
	if(nullptr == pArg)
		return E_FAIL;
	
	DYNAMIC_CAMERA_DESC* pDesc = (DYNAMIC_CAMERA_DESC*)pArg;

	m_fMouseSensitivity = pDesc->fMouseSensitivity;

	if(FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CCamera_MapTool::Priority_Tick(_float fTimeDelta)
{
}

void CCamera_MapTool::Tick(_float fTimeDelta)
{
	KeyInput(fTimeDelta);
	
	if (m_pGameInstance->Mouse_Pressing(DIM_RB))
	{
		MouseInput(fTimeDelta);
	}

	//TODO �θ��� Tick�Լ��� ȣ������� ����������� ���������� ��ü���� �����ش�.
	__super::Tick(fTimeDelta);
}

void CCamera_MapTool::Late_Tick(_float fTimeDelta)
{
}

void CCamera_MapTool::KeyInput(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_LEFTARROW))
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_RIGHTARROW))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_UPARROW))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_DOWNARROW))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_Q))
	{
		m_pTransformCom->Go_Up(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_E))
	{
		m_pTransformCom->Go_Down(fTimeDelta);
	}

}

void CCamera_MapTool::MouseInput(_float fTimeDelta)
{
	_long	MouseMove = 0;

	//TODO ���콺 �����Լ��� �������� �����Ͻ� ����, ���������� ������ �� ����� �����Ѵ�
	//! ���������ӿ� �������� ���� ���콺 ��ġ�������� ���ȼ��̳� ������������ ���� �����Ѵ�.
	//! ���� ���� �������� ȸ���� �� ���̳�, 0,1,0�� �ִ� ����� ī�޶��� UP���͸� �ִ� ������ִ�.
	//! 0,1,0 �� ������ ������ ������ ���̰� , ������ ���� �ܷ��̴� ������ ���� �� �ִ�. �غ��� �� ���ð� ����
	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
	{
		//m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), m_fMouseSensitivity * MouseMove * fTimeDelta);
		//! ���ϵǴ� ���� ������ �����Ѵ�. ������ �Ǽ��� Ÿ�ӵ�Ÿ�� �����ָ� �ȵǴ� ���콺������� �Ǽ��� �־�����
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fMouseSensitivity * MouseMove * fTimeDelta);
	}

	//TODO ī�޶� Y�� ȸ���� X���� �������� ȸ���ϴ� ���� �ƴ� ����Ʈ���� �������� ����� �׻� ��� �ٶ󺸵� ���� ȸ���� �� �� �ִ�.	
	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fMouseSensitivity * MouseMove * fTimeDelta);
	}
}

CCamera_MapTool* CCamera_MapTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CCamera_MapTool* pInstance = new CCamera_MapTool(pDevice, pContext);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CCamera_MapTool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCamera_MapTool::Clone(void* pArg)
{
	CCamera_MapTool* pInstance = new CCamera_MapTool(*this);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_MapTool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_MapTool::Free()
{
	__super::Free();
}
