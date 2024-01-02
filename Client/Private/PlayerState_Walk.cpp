#include "stdafx.h"
#include "Player.h"
#include "PlayerState_Walk.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Camera_Target.h"

CPlayerState_Walk::CPlayerState_Walk()
{
}

HRESULT CPlayerState_Walk::Initialize(CPlayer* pPlayer)
{
	if (FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;

	m_fEndTime = 0.25f; //! 일정시간 키 입력 없을시 Idle 상태로 돌아가기.


	return S_OK;
}

HRESULT CPlayerState_Walk::StartState()
{
	m_pOwnerModelCom->Set_Animation(67);
	m_pOwnerTransform->Set_SpeedPerSec(1.f);
	m_pOwnerTransform->Set_RotationPerSec(XMConvertToRadians(60.0f));

	return S_OK;
}

HRESULT CPlayerState_Walk::EndState()
{
	m_fLastInputTime = 0.f;
	m_fAccTime = 0.f;

	return S_OK;
}

void CPlayerState_Walk::Tick(const _float& fTimeDelta)
{
	KeyInput(fTimeDelta);
}

void CPlayerState_Walk::Late_Tick(const _float& fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_W) || m_pGameInstance->Key_Pressing(DIK_A)
		|| m_pGameInstance->Key_Pressing(DIK_S) || m_pGameInstance->Key_Pressing(DIK_D))
		m_bKeyPressing = true;
	else
		m_bKeyPressing = false;

	m_fAccTime += m_fLastInputTime + fTimeDelta;

	if (m_fAccTime > m_fEndTime && !m_bKeyPressing)
		m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Idle"));
}

void CPlayerState_Walk::KeyInput(const _float& fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_LSHIFT))
    {
        m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Run"));
    }

    _matrix RotationMatrix = XMMatrixIdentity();

    if (m_pGameInstance->Key_Pressing(DIK_W))
    {
        if (true == m_bNoTurn)
        {
            
            RotationMatrix = XMMatrixRotationY(XMConvertToRadians(-m_fTurnAngle));
            
            m_bLeftTurn = false;
            m_bRightTurn = false;
            m_bNoTurn = false;
        }

        m_pOwnerTransform->Go_Straight(fTimeDelta, m_pOwnerNavagation);
        m_fLastInputTime = fTimeDelta;
    }

    if (m_pGameInstance->Key_Pressing(DIK_A))
    {
        if (false == m_bLeftTurn)
        {
            if (true == m_bRightTurn)
                m_fTurnAngle = -180.0f;
            else
                m_fTurnAngle = -90.0f;

            RotationMatrix = XMMatrixRotationY(XMConvertToRadians(m_fTurnAngle));

            m_bLeftTurn = true;
            m_bRightTurn = false;
            m_bNoTurn = true;
        }
        

        m_pOwnerTransform->Go_Straight(fTimeDelta, m_pOwnerNavagation);
        m_fLastInputTime = fTimeDelta;
    }

    if (m_pGameInstance->Key_Pressing(DIK_S))
    {
        if (true == m_bRightTurn)
            RotationMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
        else if (true == m_bLeftTurn)
            RotationMatrix = XMMatrixRotationY(XMConvertToRadians(90.0f));

        //m_pOwnerTransform->Go_Straight(fTimeDelta, m_pOwnerNavagation);
        m_pOwnerTransform->Go_Backward(fTimeDelta);
        m_fLastInputTime = fTimeDelta;
    }

    if (m_pGameInstance->Key_Pressing(DIK_D))
    {
        if (false == m_bRightTurn)
        {
            if (true == m_bLeftTurn)
                m_fTurnAngle = 180.0f;
            else
                m_fTurnAngle = 90.0f;

            RotationMatrix = XMMatrixRotationY(XMConvertToRadians(m_fTurnAngle));

            m_bRightTurn = true;
            m_bLeftTurn = false;
            m_bNoTurn = true;
        }
        

        m_pOwnerTransform->Go_Straight(fTimeDelta, m_pOwnerNavagation);
        m_fLastInputTime = fTimeDelta;
    }

    if (m_pGameInstance->Key_Down(DIK_SPACE))
    {
        m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_AIR, TEXT("PlayerState_Jump"));
    }

    if (m_pGameInstance->Mouse_Down(DIM_LB))
    {
        m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Attack1"));
    }

    // 새로운 회전 행렬을 적용
    _float4x4 CurrentMatrix = m_pOwnerTransform->Get_WorldFloat4x4();
    XMMATRIX newRotation = XMMatrixMultiply(XMLoadFloat4x4(&CurrentMatrix), RotationMatrix);
    XMStoreFloat4x4(&CurrentMatrix, newRotation);
    m_pOwnerTransform->Set_WorldFloat4x4(CurrentMatrix);
}

CPlayerState_Walk* CPlayerState_Walk::Create(CPlayer* pPlayer)
{
	CPlayerState_Walk* pInstance = new CPlayerState_Walk();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_Walk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_Walk::Free()
{
	__super::Free();
}
