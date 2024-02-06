#include "stdafx.h"
#include "Player.h"
#include "PlayerState_BloodyMode_Idle.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Camera_Target.h"

CPlayerState_BloodyMode_Idle::CPlayerState_BloodyMode_Idle()
{
}

HRESULT CPlayerState_BloodyMode_Idle::Initialize(CPlayer* pPlayer)
{
	if(FAILED(__super::Initialize(pPlayer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerState_BloodyMode_Idle::StartState()
{
	m_pOwnerModelCom->Set_Animation(446);
	m_pOwnerModelCom->Set_Loop(true);
	m_pOwnerModelCom->Root_MotionEnd();
	return S_OK;
}

HRESULT CPlayerState_BloodyMode_Idle::EndState()
{
	
	
	m_fAccTime = 0.f;
	m_fLastInputTime = 0.f;

	return S_OK;
}

void CPlayerState_BloodyMode_Idle::Tick(const _float& fTimeDelta)
{
	KeyInput(fTimeDelta);
}

void CPlayerState_BloodyMode_Idle::KeyInput(const _float& fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_LSHIFT))
    {
        m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_BloodMode_Run"));
    }

    if (m_pGameInstance->Key_Pressing(DIK_W))
    {
        if (m_pGameInstance->Key_Pressing(DIK_SPACE))
        {
            m_ePrevDir = m_eCurrentDir;
            m_eCurrentDir = CPlayerState_Base::FRONT;
            if (m_ePrevDir != m_eCurrentDir)
            {
                Vertical_Camera_Rotate();
            }

            m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_DashFront"));


        }


        m_ePrevDir = m_eCurrentDir;
        m_eCurrentDir = CPlayerState_Base::FRONT;

        if (m_ePrevDir != m_eCurrentDir)
        {
            Vertical_Camera_Rotate();
        }

        _float3 vCamLook;
        XMStoreFloat3(&vCamLook, m_pOwnerCam->Get_Transform()->Get_State(CTransform::STATE_LOOK));

        m_pOwnerTransform->Go_Player_Straight(fTimeDelta, vCamLook, m_pOwnerNavagation);
        m_fLastInputTime = fTimeDelta;
    }

    if (m_pGameInstance->Key_Pressing(DIK_A))
    {

        if (m_pGameInstance->Key_Pressing(DIK_SPACE))
        {
            m_ePrevDir = m_eCurrentDir;
            m_eCurrentDir = CPlayerState_Base::LEFT;

            if (m_ePrevDir != m_eCurrentDir)
            {
                Horizon_Camera_Rotate();
            }
            m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_DashLeft"));

        }


        m_ePrevDir = m_eCurrentDir;
        m_eCurrentDir = CPlayerState_Base::LEFT;

        if (m_ePrevDir != m_eCurrentDir)
        {
            Horizon_Camera_Rotate();
        }

        _vector vCamLook = m_pOwnerCam->Get_Transform()->Get_State(CTransform::STATE_LOOK);
        _vector vCamUp = m_pOwnerCam->Get_Transform()->Get_State(CTransform::STATE_UP);

        _float3 vCamLeft;
        XMStoreFloat3(&vCamLeft, XMVector3Cross(vCamLook, vCamUp));



        m_pOwnerTransform->Go_Player_Straight(fTimeDelta, vCamLeft, m_pOwnerNavagation);
        m_fLastInputTime = fTimeDelta;
    }

    if (m_pGameInstance->Key_Pressing(DIK_S))
    {

        if (m_pGameInstance->Key_Pressing(DIK_SPACE))
        {
            m_ePrevDir = m_eCurrentDir;
            m_eCurrentDir = CPlayerState_Base::BACK;
            if (m_ePrevDir != m_eCurrentDir)
            {
                Vertical_Camera_Rotate();
            }

            m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_DashBack"));

        }

        m_ePrevDir = m_eCurrentDir;
        m_eCurrentDir = CPlayerState_Base::BACK;

        if (m_ePrevDir != m_eCurrentDir)
        {
            Vertical_Camera_Rotate();
        }

        m_pOwnerTransform->Go_Straight(fTimeDelta, m_pOwnerNavagation);
        m_fLastInputTime = fTimeDelta;
    }

    if (m_pGameInstance->Key_Pressing(DIK_D))
    {

        if (m_pGameInstance->Key_Pressing(DIK_SPACE))
        {
            m_ePrevDir = m_eCurrentDir;
            m_eCurrentDir = CPlayerState_Base::RIGHT;

            if (m_ePrevDir != m_eCurrentDir)
            {
                Horizon_Camera_Rotate();
            }
            m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_DashRight"));
        }

        m_ePrevDir = m_eCurrentDir;
        m_eCurrentDir = CPlayerState_Base::RIGHT;

        if (m_ePrevDir != m_eCurrentDir)
        {
            Horizon_Camera_Rotate();
        }

        _vector vCamLook = m_pOwnerCam->Get_Transform()->Get_State(CTransform::STATE_LOOK);
        _vector vCamUp = m_pOwnerCam->Get_Transform()->Get_State(CTransform::STATE_UP);

        _float3 vCamRight;
        XMStoreFloat3(&vCamRight, XMVector3Cross(vCamUp, vCamLook));


        m_pOwnerTransform->Go_Player_Straight(fTimeDelta, vCamRight, m_pOwnerNavagation);
        m_fLastInputTime = fTimeDelta;
    }

    //if (m_pGameInstance->Key_Down(DIK_SPACE))
    //{
    //    m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_AIR, TEXT("PlayerState_Jump"));
    //}

    if (m_pGameInstance->Mouse_Down(DIM_LB))
    {
        m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_BloodyMode_Attack1"));
    }
}

CPlayerState_BloodyMode_Idle* CPlayerState_BloodyMode_Idle::Create(CPlayer* pPlayer)
{
	CPlayerState_BloodyMode_Idle* pInstance = new CPlayerState_BloodyMode_Idle();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerState_BloodyMode_Idle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerState_BloodyMode_Idle::Free()
{
	__super::Free();
}
