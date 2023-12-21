#include "stdafx.h"
#include "Camera_Dynamic.h"
#include "GameInstance.h"

CCamera_Dynamic::CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCamera(pDevice,pContext)
{
}

CCamera_Dynamic::CCamera_Dynamic(const CCamera_Dynamic& rhs)
	:CCamera(rhs)
{
}

HRESULT CCamera_Dynamic::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelID = eLevel;
	return S_OK;
}

HRESULT CCamera_Dynamic::Initialize(void* pArg)
{
	if(nullptr == pArg)
		return E_FAIL;
	
	DYNAMIC_CAMERA_DESC* pDesc = (DYNAMIC_CAMERA_DESC*)pArg;

	m_fMouseSensitivity = pDesc->fMouseSensitivity;

	if(FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Dynamic::Priority_Tick(_float fTimeDelta)
{
}

void CCamera_Dynamic::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_F5))
		m_bAdmin = true;

	if (m_pGameInstance->Key_Down(DIK_F6))
		m_bAdmin = false;

	if (!m_bAdmin)
	{
		KeyInput(fTimeDelta);
		MouseInput(fTimeDelta);
	}

	//TODO �θ��� Tick�Լ��� ȣ������� ����������� ���������� ��ü���� �����ش�.
	__super::Tick(fTimeDelta);
}

void CCamera_Dynamic::Late_Tick(_float fTimeDelta)
{
}

void CCamera_Dynamic::KeyInput(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_A))
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}


	if (m_pGameInstance->Key_Pressing(DIK_D))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_W))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

}

void CCamera_Dynamic::MouseInput(_float fTimeDelta)
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

CCamera_Dynamic* CCamera_Dynamic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CCamera_Dynamic* pInstance = new CCamera_Dynamic(pDevice, pContext);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CCamera_Dynamic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCamera_Dynamic::Clone(void* pArg)
{
	CCamera_Dynamic* pInstance = new CCamera_Dynamic(*this);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Dynamic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Dynamic::Free()
{
	__super::Free();
}
