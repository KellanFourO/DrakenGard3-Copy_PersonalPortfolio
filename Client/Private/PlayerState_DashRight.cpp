#include "stdafx.h"
#include "Player.h"
#include "PlayerState_DashRight.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Camera_Target.h"
#include "Animation.h"

CPlayerState_DashRight::CPlayerState_DashRight()
{
}

HRESULT CPlayerState_DashRight::Initialize(CPlayer* pPlayer)
{
    if (FAILED(__super::Initialize(pPlayer)))
        return E_FAIL;

    m_fEndTime = 0.25f; //! 일정시간 키 입력 없을시 Idle 상태로 돌아가기.


    return S_OK;
}

HRESULT CPlayerState_DashRight::StartState()
{
    m_pOwnerModelCom->Set_Animation(80);

    m_pOwnerModelCom->Set_AnimationSpeed(5.0f);
    m_pOwnerTransform->Set_SpeedPerSec(5.f);

    m_pOwnerModelCom->Set_Loop(false);
    m_pOwnerModelCom->Root_MotionStart();

    m_pOwnerTransform->Set_RotationPerSec(XMConvertToRadians(60.0f));

    _float3 vCamLook;
    XMStoreFloat3(&vCamLook, m_pOwnerCam->Get_Transform()->Get_State(CTransform::STATE_LOOK));


    m_pOwnerTransform->Look_At_CamLook(vCamLook);
    //_float3 vAddLookPos = m_pOwnerTransform->Get_MoveAxisPos(10.f, CTransform::STATE_LOOK, true, vCamLook);

    m_pOwnerRigidBody->Add_Force(_float3(300.f, 0.f, 0.f), CRigidBody::FORCE_MODE::IMPULSE);

    return S_OK;
}

HRESULT CPlayerState_DashRight::EndState()
{
    m_fLastInputTime = 0.f;
    m_fAccTime = 0.f;
    m_pOwnerModelCom->Reset_AnimationSpeed();

    m_pOwnerRigidBody->Clear_NetPower();

    return S_OK;
}

void CPlayerState_DashRight::Tick(const _float& fTimeDelta)
{
 
}

void CPlayerState_DashRight::Late_Tick(const _float& fTimeDelta)
{
    if (true == m_pOwnerModelCom->Get_CurrentAnimation()->Get_Finished())
    {
        m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, m_pOwnerStateCom->Get_PrevStateTag());
    }
}


CPlayerState_DashRight* CPlayerState_DashRight::Create(CPlayer* pPlayer)
{
    CPlayerState_DashRight* pInstance = new CPlayerState_DashRight();

    /* 원형객체를 초기화한다.  */
    if (FAILED(pInstance->Initialize(pPlayer)))
    {
        MSG_BOX("Failed to Created : CPlayerState_DashRight");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPlayerState_DashRight::Free()
{
    __super::Free();
}
