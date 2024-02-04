#include "stdafx.h"
#include "Camera_Target.h"
#include "GameInstance.h"

#include "Transform.h"
#include "Player.h"
#include "Boss_EN131.h"

CCamera_Target::CCamera_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCamera(pDevice,pContext)
{
}

CCamera_Target::CCamera_Target(const CCamera_Target& rhs)
	:CCamera(rhs)
{
}

void CCamera_Target::On_Shake(_float fShakePower, _float fShakeDuration)
{
	m_bShake = true;
	m_fShakePower = fShakePower;
	m_fShakeDuration = fShakeDuration;
}

void CCamera_Target::CameraShake(_float fTimeDelta)
{
	// 저장된 원래 카메라 오프셋 및 위치를 복사
	_float3 vOriginalOffset = m_vOffset;
	_vector vOriginalPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


	m_fShakeTimeAcc += fTimeDelta;

	if (m_fShakeTimeAcc > m_fShakeDuration)
	{
		m_fShakeTimeAcc = 0.f;
		m_bShake = false;
		m_fShakePower = 0.f;
		m_fShakeDuration = 0.f;
		// 흔들림이 끝난 후 원래 위치로 되돌림
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vOriginalPosition);
	}
	else
	{
			m_RandomNumber = mt19937_64(m_RandomDevice());

			uniform_real_distribution<float>	RandomShakeX(-m_fShakePower, m_fShakePower);
			uniform_real_distribution<float>	RandomShakeY(-m_fShakePower, m_fShakePower);
			uniform_real_distribution<float>	RandomShakeZ(-m_fShakePower, m_fShakePower);
		// 흔들림 강도를 랜덤하게 적용
		_float3 vShakeOffset = 
		{
			RandomShakeX(m_RandomNumber),
			RandomShakeY(m_RandomNumber),
			RandomShakeZ(m_RandomNumber)
		};

		// 현재 흔들린 위치 계산
		_vector vShakePosition = vOriginalPosition + XMLoadFloat3(&vShakeOffset);

		// 카메라 위치를 흔들린 위치로 설정
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vShakePosition);
	}
		

	

	
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
	
	m_RandomNumber = mt19937_64(m_RandomDevice());

	TARGET_CAMERA_DESC* pDesc = (TARGET_CAMERA_DESC*)pArg;

	m_fMouseSensitivity = pDesc->fMouseSensitivity;
	m_pTarget = pDesc->pTarget;

	if(nullptr == m_pTarget)
		return E_FAIL;

	if(FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fSpringConstant = 300.f;
	m_fDampConstant = 2.0f * sqrt(m_fSpringConstant);

	m_vOffset = m_pTarget->Get_Offset();
	
	//XMStoreFloat3(&m_vOffset, XMVectorSet(0.f, 5.f, -5.f, 0.f));

	CTransform* pTargetTransform = m_pTarget->Get_Transform();

	_vector vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);
	//!XMStoreFloat3(&m_vTargetPos, vTargetPos);

	vTargetPos.m128_f32[1] += 1.f;
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
	if (m_tCutScene.bCutSin == false && m_pTarget != nullptr)
	{
		if (m_pGameInstance->Key_Down(DIK_R))
			m_bMouseFix = !m_bMouseFix;

		if (true == m_bMouseFix)
			m_pGameInstance->Mouse_Fix();

		
		//if (typeid(*m_pTarget) == typeid(CPlayer))
		//{
		//	
		//}
		//
		//LEVEL eTargetLevel = dynamic_cast<CPlayer*>(m_pTarget)->Get_LevelID();
		//
		//if (eTargetLevel != LEVEL_TOOL)
		//{

		
			_vector vActualPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);  //! 이게 현재 위치
			XMStoreFloat3(&m_vActualPos, vActualPos);

			CTransform* pTargetTransform = m_pTarget->Get_Transform();

			_vector vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION); //! 타겟 위치
			vTargetPos.m128_f32[1] += 1.f;

			//! 타겟의 위치는 계속 변경되니 다시 이상적인 위치를 구해주자.
			_vector vIdealPosition;
			vIdealPosition = vTargetPos + MouseInput(fTimeDelta); //! 이상적인 위치
			

			//_vector vLookIdealPosition;
			//vLookIdealPosition = vTargetPos;

			//! 이상적인 위치에서 실제 위치로 향하는 방향 벡터를 구하자.
			_vector vDisplacement = vActualPos - vIdealPosition;
			_vector vLookDisplacement = vTargetPos + vDisplacement;

			_vector vSpringAccel = (-m_fSpringConstant * vDisplacement) - (m_fDampConstant * XMLoadFloat3(&m_vVelocity));
			//_vector vLookSpringAccel = (-m_fSpringConstant * vLookDisplacement) - (m_fDampConstant * XMLoadFloat3(&m_vLookVelocity));

			XMStoreFloat3(&m_vVelocity, (XMLoadFloat3(&m_vVelocity) + (vSpringAccel * fTimeDelta)));
			//XMStoreFloat3(&m_vLookVelocity, (XMLoadFloat3(&m_vLookVelocity) + (vLookSpringAccel * fTimeDelta)));

			vActualPos += (XMLoadFloat3(&m_vVelocity) * fTimeDelta); //! 실제 위치
			//vLookActualPos += (XMLoadFloat3(&m_vLookVelocity) * fTimeDelta);


			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vActualPos); //! 실제위치 셋.
			//XMStoreFloat3(&m_vTargetPos, vLookActualPos);

			//m_pTransformCom->Look_At(vLookDisplacement);
			m_pTransformCom->Look_At(vTargetPos);
			//! 타겟 포지션 룩엣도 보정을 해줘야한다.

			if(true == m_bShake)
				CameraShake(fTimeDelta);

			//TODO 부모의 Tick함수를 호출해줘야 뷰투영행렬을 파이프라인 객체에게 던져준다.
			
		//}

		__super::Tick(fTimeDelta);
	}
	else if (m_tCutScene.bCutSin == true)
	{
		Start_CutScene(fTimeDelta);
		__super::Tick(fTimeDelta);
	}


	
	
}

void CCamera_Target::Late_Tick(_float fTimeDelta)
{
	
}

void CCamera_Target::Start_CutScene(_float fTimeDelta)
{

	//!typedef struct tagCutSceneDesc
	//!{
	//!	_bool			bCutSin = false; //! true가 되면
	//!	_float3			vStartPos = {}; //! 시작위치
	//!	_float3			vChasePos = {}; //! 지정한 위치로 천천히 추적
	//!	_float			fStopRange = 0.f; //! 추적을 멈출 범위
	//!	_float			fChaseSpeed = 0.f; //! 추적 스피드
	//!
	//!	CGameObject* pChaseTarget = nullptr;
	//!}CUTSCENE_DESC;
	
	if (false == m_bSaveOriginPos)
	{
		XMStoreFloat4(&m_vOriginPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_bSaveOriginPos = true;

		m_fSpringConstant = 100.f;
		m_fDampConstant = 2.0f * sqrt(m_fSpringConstant);
		
		_vector vStartPos = XMLoadFloat3(&m_tCutScene.vStartPos);
		vStartPos.m128_f32[1] += 1.f;
		vStartPos.m128_f32[3] = 1.f;

		vStartPos.m128_f32[2] = m_tCutScene.vChasePos.z;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vStartPos);
		m_pTransformCom->Look_At(XMVectorSetW(XMLoadFloat3(&m_tCutScene.vChasePos),1.f));
		Init_CutScene();

		XMStoreFloat3(&m_vVelocity, XMVectorZero());
	}

	_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vEndPos = m_tCutScene.pChaseTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	vEndPos.m128_f32[3] = 1.f;

	// 현재 위치에서 목표 위치까지의 거리 계산
	_float fDistance = XMVectorGetX(XMVector3Length(vEndPos - vCurrentPos));

	// 현재 프레임에서 이동할 속도
	_float fSpeed = m_tCutScene.fChaseSpeed * 1.5f * fTimeDelta;

	// 거리에 따라 가속도 적용
	fSpeed *= fDistance / m_tCutScene.fStopRange;

	_vector vTargetPos = m_tCutScene.pChaseTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_tCutScene.pChaseTarget->Get_Offset());

	if (false == InRange(fDistance, 0.f, m_tCutScene.fStopRange, "[]"))
	{
		m_pTransformCom->Go_Target_Speed(vEndPos, fTimeDelta, fSpeed, m_tCutScene.fStopRange);

		
		
	}
	else
	{
		//_vector vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		//vCameraPos.m128_f32[0] = vTargetPos.m128_f32[0];
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCameraPos);

		m_pTarget = m_tCutScene.pChaseTarget;
		m_bTargetChange = true;
		
		m_fSpringConstant = 300.f;
		m_fDampConstant = 2.0f * sqrt(m_fSpringConstant);
		//!XMStoreFloat3(&m_vTargetPos, vTargetPos);

		//!m_vCameraOffset = { 0.f, 10.f, -10.f };
		//!m_vJumpOffset = { 0.f, 24.f, -25.f };
		//m_pTransformCom->RotationAroundPoint()
		_vector vActualPos = vTargetPos + XMLoadFloat3(&m_tCutScene.pChaseTarget->Get_Offset());

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vActualPos);
		m_fMouseX = m_tCutScene.fStartAngle;

		XMStoreFloat3(&m_vVelocity, XMVectorZero());

		m_pTarget->SetStartAppeal(true);

		

		Reset_CutScene();
		m_tCutScene.bCutSin = false;
	}

	
	
	
}

void CCamera_Target::Init_CutScene()
{
	//m_pTransformCom->Set_SpeedPerSec(m_tCutScene.fChaseSpeed);
	//m_pTransformCom->Set_RotationPerSec(XMConvertToRadians(60.f));
}

void CCamera_Target::Reset_CutScene()
{

	m_tCutScene = {};
	m_bTargetChange = false;
}

void CCamera_Target::Return_Player()
{
	if (typeid(*m_pTarget) == typeid(CBoss_EN131))
	{
		if (true == dynamic_cast<CBoss_EN131*>(m_pTarget)->Get_Appear())
		{
			CCamera_Target::CUTSCENE_DESC Desc;

			Desc.fChaseSpeed = 90.f;
			Desc.fStopRange = 10.f;
			Desc.pChaseTarget = m_pOriginTarget;
			XMStoreFloat3(&Desc.vStartPos, m_pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION));
			Desc.vStartPos.y += 4.f;
			XMStoreFloat3(&Desc.vChasePos, Desc.pChaseTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION));

			Set_OffSet(m_pOriginTarget->Get_Offset());
			Set_CutSceneDesc(&Desc);
		}
	}
	
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
		if (typeid(*m_pTarget) == typeid(CPlayer))
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

	/* 원형객체를 초기화한다.  */
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

	/* 원형객체를 초기화한다.  */
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
