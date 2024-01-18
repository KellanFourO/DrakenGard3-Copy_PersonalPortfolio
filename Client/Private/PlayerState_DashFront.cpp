#include "stdafx.h"
#include "Player.h"
#include "PlayerState_DashFront.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Camera_Target.h"
#include "Animation.h"

CPlayerState_DashFront::CPlayerState_DashFront()
{
}

HRESULT CPlayerState_DashFront::Initialize(CPlayer* pPlayer)
{
    if (FAILED(__super::Initialize(pPlayer)))
        return E_FAIL;

    m_fEndTime = 0.25f; //! 일정시간 키 입력 없을시 Idle 상태로 돌아가기.


    return S_OK;
}

HRESULT CPlayerState_DashFront::StartState()
{
    m_pOwnerModelCom->Set_Animation(78);

    m_pOwnerModelCom->Set_AnimationSpeed(5.0f);
    m_pOwnerTransform->Set_SpeedPerSec(5.f);

    m_pOwnerModelCom->Set_Loop(false);
    m_pOwnerModelCom->Root_MotionStart();

    m_pOwnerTransform->Set_RotationPerSec(XMConvertToRadians(60.0f));

    _float3 vCamLook;
    XMStoreFloat3(&vCamLook, m_pOwnerCam->Get_Transform()->Get_State(CTransform::STATE_LOOK));
    
    m_pOwnerTransform->Look_At_CamLook(vCamLook);
    //_float3 vAddLookPos = m_pOwnerTransform->Get_MoveAxisPos(10.f, CTransform::STATE_LOOK, true, vCamLook);

    m_pOwnerRigidBody->Add_Force(_float3(0.f, 0.f, 300.f), CRigidBody::FORCE_MODE::IMPULSE);
    return S_OK;
}

HRESULT CPlayerState_DashFront::EndState()
{
    m_fLastInputTime = 0.f;
    m_fAccTime = 0.f;
    m_pOwnerModelCom->Reset_AnimationSpeed();
    m_pOwnerRigidBody->Clear_NetPower();


    return S_OK;
}

void CPlayerState_DashFront::Tick(const _float& fTimeDelta)
{
}

void CPlayerState_DashFront::Late_Tick(const _float& fTimeDelta)
{
    if (true == m_pOwnerModelCom->Get_CurrentAnimation()->Get_Finished())
        m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, m_pOwnerStateCom->Get_PrevStateTag());
}


CPlayerState_DashFront* CPlayerState_DashFront::Create(CPlayer* pPlayer)
{
    CPlayerState_DashFront* pInstance = new CPlayerState_DashFront();

    /* 원형객체를 초기화한다.  */
    if (FAILED(pInstance->Initialize(pPlayer)))
    {
        MSG_BOX("Failed to Created : CPlayerState_DashFront");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPlayerState_DashFront::Free()
{
    __super::Free();
}
