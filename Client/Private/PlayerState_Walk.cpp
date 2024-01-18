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
#include "Animation.h"

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
    
    
    

    m_pOwnerModelCom->Set_AnimationSpeed(1.5f);
	m_pOwnerTransform->Set_SpeedPerSec(5.f);
	m_pOwnerTransform->Set_RotationPerSec(XMConvertToRadians(60.0f));

	return S_OK;
}

HRESULT CPlayerState_Walk::EndState()
{
	m_fLastInputTime = 0.f;
	m_fAccTime = 0.f;
    m_pOwnerModelCom->Reset_AnimationSpeed();

 
    

	return S_OK;
}

void CPlayerState_Walk::Tick(const _float& fTimeDelta)
{
    MouseInput(fTimeDelta);
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

    if (m_pGameInstance->Key_Pressing(DIK_W))
    {
        if (m_pGameInstance->Key_Pressing(DIK_SPACE))
        {
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
        m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Attack1"));
    }


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
