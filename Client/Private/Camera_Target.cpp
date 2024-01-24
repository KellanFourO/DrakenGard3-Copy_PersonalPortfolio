#include "stdafx.h"
#include "Camera_Target.h"
#include "GameInstance.h"

#include "Transform.h"
#include "Player.h"

CCamera_Target::CCamera_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCamera(pDevice,pContext)
{
}

CCamera_Target::CCamera_Target(const CCamera_Target& rhs)
	:CCamera(rhs)
{
}

HRESULT CCamera_Target::Initialize_Prototype(LEVEL eLevel)
{
	m_eCurrentLevelID = eLevel;
	return S_OK;
}

HRESULT CCamera_Target::Initialize(void* pArg)
{
	if(nullptr == pArg)
		return E_FAIL;
	
	TARGET_CAMERA_DESC* pDesc = (TARGET_CAMERA_DESC*)pArg;

	m_fMouseSensitivity = pDesc->fMouseSensitivity;
	m_pTarget = pDesc->pTarget;

	if(nullptr == m_pTarget)
		return E_FAIL;

	if(FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fSpringConstant = 500.f;
	m_fDampConstant = 2.0f * sqrt(m_fSpringConstant);

	XMStoreFloat3(&m_vOffset, XMVectorSet(0.f, 5.f, -5.f, 0.f));

	CTransform* pTargetTransform = m_pTarget->Get_Transform();

	_vector vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);
	//!XMStoreFloat3(&m_vTargetPos, vTargetPos);

	_vector vActualPos = vTargetPos + XMLoadFloat3(&m_vOffset);
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vActualPos);

	XMStoreFloat3(&m_vVelocity, XMVectorZero());

	return S_OK;
}

void CCamera_Target::Priority_Tick(_float fTimeDelta)
{
}

void CCamera_Target::Tick(_float fTimeDelta)
{
	
	if (m_pTarget != nullptr)
	{

		if (m_pGameInstance->Key_Down(DIK_R))
			m_bMouseFix = !m_bMouseFix;

		if (true == m_bMouseFix)
			m_pGameInstance->Mouse_Fix();

		LEVEL eTargetLevel = dynamic_cast<CPlayer*>(m_pTarget)->Get_LevelID();

		if (eTargetLevel != LEVEL_TOOL)
		{


			_vector vActualPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);  //! �̰� ���� ��ġ
			XMStoreFloat3(&m_vActualPos, vActualPos);

			CTransform* pTargetTransform = m_pTarget->Get_Transform();

			_vector vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION); //! Ÿ�� ��ġ


			//! Ÿ���� ��ġ�� ��� ����Ǵ� �ٽ� �̻����� ��ġ�� ��������.
			_vector vIdealPosition;
			vIdealPosition = vTargetPos + MouseInput(fTimeDelta); //! �̻����� ��ġ

			//_vector vLookIdealPosition;
			//vLookIdealPosition = vTargetPos;

			//! �̻����� ��ġ���� ���� ��ġ�� ���ϴ� ���� ���͸� ������.
			_vector vDisplacement = vActualPos - vIdealPosition;
			_vector vLookDisplacement = vTargetPos + vDisplacement;

			_vector vSpringAccel = (-m_fSpringConstant * vDisplacement) - (m_fDampConstant * XMLoadFloat3(&m_vVelocity));
			//_vector vLookSpringAccel = (-m_fSpringConstant * vLookDisplacement) - (m_fDampConstant * XMLoadFloat3(&m_vLookVelocity));

			XMStoreFloat3(&m_vVelocity, (XMLoadFloat3(&m_vVelocity) + (vSpringAccel * fTimeDelta)));
			//XMStoreFloat3(&m_vLookVelocity, (XMLoadFloat3(&m_vLookVelocity) + (vLookSpringAccel * fTimeDelta)));

			vActualPos += (XMLoadFloat3(&m_vVelocity) * fTimeDelta); //! ���� ��ġ
			//vLookActualPos += (XMLoadFloat3(&m_vLookVelocity) * fTimeDelta);


			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vActualPos); //! ������ġ ��.
			//XMStoreFloat3(&m_vTargetPos, vLookActualPos);

			//m_pTransformCom->Look_At(vLookDisplacement);
			m_pTransformCom->Look_At(vTargetPos);
			//! Ÿ�� ������ �迧�� ������ ������Ѵ�.


			//TODO �θ��� Tick�Լ��� ȣ������� ����������� ���������� ��ü���� �����ش�.
			__super::Tick(fTimeDelta);
		}
	}
	
}

void CCamera_Target::Late_Tick(_float fTimeDelta)
{
	
}

void CCamera_Target::KeyInput(_float fTimeDelta)
{
	if (true == m_bAdmin)
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

}

_vector CCamera_Target::MouseInput(_float fTimeDelta)
{
	_long	MouseMove = 0;

	_vector vQuaternion;
	_matrix RotationMatrix;
	
	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
	{
		m_fMouseX += XMConvertToRadians(MouseMove * m_fMouseSensitivity);
		
		//_float fAngle = XMConvertToRadians(MouseMove * XM_PI * fTimeDelta);6
		//m_pTarget->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fAngle);
		m_pTarget->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(MouseMove * fTimeDelta));
	}
	
	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
	{

		m_fMouseY += XMConvertToRadians(MouseMove * m_fMouseSensitivity);
		m_fMouseY = max(-XM_PIDIV2 * 0.4f, min(XM_PIDIV2 * 0.4f, m_fMouseY));
	}

	vQuaternion = XMQuaternionRotationRollPitchYaw(m_fMouseY, m_fMouseX, 0.f);
	vQuaternion = XMQuaternionNormalize(vQuaternion);
	RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);


	return XMVector3TransformNormal(XMLoadFloat3(&m_vOffset), RotationMatrix);
}


CCamera_Target* CCamera_Target::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CCamera_Target* pInstance = new CCamera_Target(pDevice, pContext);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CCamera_Target");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCamera_Target::Clone(void* pArg)
{
	CCamera_Target* pInstance = new CCamera_Target(*this);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Target");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Target::Free()
{
	__super::Free();
}
