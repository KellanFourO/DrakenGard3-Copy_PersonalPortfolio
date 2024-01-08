#include "stdafx.h"

#include "Player.h"
#include "PlayerState_Base.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "Bone.h"
#include "Model.h"
#include "Animation.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Navigation.h"
#include "StateMachine.h"
#include "Camera_Target.h"


CPlayerState_Base::CPlayerState_Base()
{
}

HRESULT CPlayerState_Base::Initialize(CPlayer* pPlayer)
{
	
	m_pOwnerTransform = dynamic_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	m_pOwnerStateCom =	dynamic_cast<CStateMachine*>(pPlayer->Find_Component(TEXT("Com_StateMachine")));
	m_pOwnerNavagation = dynamic_cast<CNavigation*>(pPlayer->Find_Component(TEXT("Com_Navigation")));
	m_pOwnerRigidBody = dynamic_cast<CRigidBody*>(pPlayer->Find_Component(TEXT("Com_RigidBody")));
	m_pOwnerModelCom = dynamic_cast<CModel*>(pPlayer->Find_PartObject(TEXT("Part_Body"))->Find_Component(TEXT("Com_Model")));
	m_pOwnerCam = pPlayer->Get_Cam();

	if(__super::Initialize(m_pOwnerModelCom))
		return E_FAIL;
	if (FAILED(AddRefIfNotNull(m_pOwnerStateCom)))
		return E_FAIL;
	if (FAILED(AddRefIfNotNull(m_pOwnerTransform)))
		return E_FAIL;
	if (FAILED(AddRefIfNotNull(m_pOwnerNavagation)))
		return E_FAIL;
	if (FAILED(AddRefIfNotNull(m_pOwnerRigidBody)))
		return E_FAIL;
	if (FAILED(AddRefIfNotNull(m_pOwnerCam)))
		return E_FAIL;


	
	
	return S_OK;
}

void CPlayerState_Base::KeyInput(const _float& fTimeDelta)
{
}

void CPlayerState_Base::MouseInput(const _float& fTimeDelta)
{
	_long	MouseMove = 0;

	_vector vQuaternion;
	_matrix RotationMatrix;

	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
	{
		m_fMouseX += XMConvertToRadians(MouseMove * m_fMouseSensitivity);


		//_float fAngle = XMConvertToRadians(MouseMove * m_fMouseSensitivity);
		//m_pTarget->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fAngle);
	}

	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
	{

		m_fMouseY += XMConvertToRadians(MouseMove * m_fMouseSensitivity);
		m_fMouseY = max(-XM_PIDIV2 * 0.4f, min(XM_PIDIV2 * 0.4f, m_fMouseY));
	}

	vQuaternion = XMQuaternionRotationRollPitchYaw(m_fMouseY, m_fMouseX, 0.f);
	vQuaternion = XMQuaternionNormalize(vQuaternion);
	RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

	_vector vPos = m_pOwnerTransform->Get_State(CTransform::STATE_POSITION);

	
}

void CPlayerState_Base::RootMotion()
{
	m_pOwnerModelCom->Root_Motion(m_pOwnerTransform);
}


void CPlayerState_Base::Vertical_Camera_Rotate()
{

	CTransform* pCamTransform = m_pOwnerCam->Get_Transform();

	_vector vCamRight = pCamTransform->Get_State(CTransform::STATE_RIGHT);

	_float fRoationAngle;

	if (m_eCurrentDir == CPlayerState_Base::FRONT)
		fRoationAngle = -45.f;
	else
		fRoationAngle = 45.f;

	m_pOwnerTransform->RotationOfCameraDir(vCamRight, XMConvertToRadians(fRoationAngle)); //! XMConvertToRadian을 해봤더니 무조건 아래 방향만 보게된다.
}

void CPlayerState_Base::Vertical_Camera_RotateTest()
{
	
}

void CPlayerState_Base::Horizon_Camera_Rotate()
{
	CTransform* pCamTransform = m_pOwnerCam->Get_Transform();

	_vector vCamLook = pCamTransform->Get_State(CTransform::STATE_LOOK);

	_float fRoationAngle;

	if(m_eCurrentDir == CPlayerState_Base::LEFT)
		fRoationAngle = -45.f;
	else
		fRoationAngle = 45.f;

	m_pOwnerTransform->RotationOfCameraDir(vCamLook, XMConvertToRadians(fRoationAngle)); //! XMConvertToRadian을 해봤더니 무조건 아래 방향만 보게된다.
}





void CPlayerState_Base::Free()
{
	__super::Free();

	Safe_Release(m_pOwnerStateCom);
	Safe_Release(m_pOwnerTransform);
	Safe_Release(m_pOwnerRigidBody);
	Safe_Release(m_pOwnerNavagation);
	Safe_Release(m_pOwnerCam);
}
