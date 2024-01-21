#include "stdafx.h"
#include "Player.h"
#include "PlayerState_DashLeft.h"
#include "PlayerPart_Body.h"

#include "GameInstance.h"
#include "StateMachine.h"
#include "Model.h"
#include "Transform.h"
#include "Navigation.h"
#include "RigidBody.h"
#include "Camera_Target.h"
#include "Animation.h"

CPlayerState_DashLeft::CPlayerState_DashLeft()
{
}

HRESULT CPlayerState_DashLeft::Initialize(CPlayer* pPlayer)
{
    if (FAILED(__super::Initialize(pPlayer)))
        return E_FAIL;

    m_fEndTime = 0.25f; //! 일정시간 키 입력 없을시 Idle 상태로 돌아가기.


    return S_OK;
}

HRESULT CPlayerState_DashLeft::StartState()
{
    m_pOwnerModelCom->Set_Animation(81);

    m_pOwnerModelCom->Set_AnimationSpeed(5.0f);
    m_pOwnerTransform->Set_SpeedPerSec(5.f);

    m_pOwnerModelCom->Set_Loop(false);
    m_pOwnerModelCom->Root_MotionStart();

    m_pOwnerTransform->Set_RotationPerSec(XMConvertToRadians(60.0f));


    //CTransform* pCamTransform = m_pOwnerCam->Get_Transform();
    //
    //_vector vCamLook = pCamTransform->Get_State(CTransform::STATE_LOOK);
    //
    //m_pOwnerTransform->RotationOfCameraDir(vCamLook, -45.f);

    _float3 vAddFos = _float3(0.f, 0.f, -300.f);
    _float magnitude = XMVectorGetZ(XMVector3Length(XMLoadFloat3(&vAddFos)));
    _vector vLook = m_pOwnerTransform->Get_State(CTransform::STATE_LOOK);
    _vector vForceDir = vLook * magnitude;
    _float3 vCalcPos;
    XMStoreFloat3(&vCalcPos, vForceDir);


    m_pOwnerRigidBody->Add_Force(vCalcPos, CRigidBody::FORCE_MODE::IMPULSE);
    return S_OK;
}

HRESULT CPlayerState_DashLeft::EndState()
{
    m_fLastInputTime = 0.f;
    m_fAccTime = 0.f;
    m_pOwnerModelCom->Reset_AnimationSpeed();

    m_pOwnerRigidBody->Clear_NetPower();


    return S_OK;
}

void CPlayerState_DashLeft::Tick(const _float& fTimeDelta)
{
    
}
    

void CPlayerState_DashLeft::Late_Tick(const _float& fTimeDelta)
{
    if (true == m_pOwnerModelCom->Get_CurrentAnimation()->Get_Finished())
        m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, m_pOwnerStateCom->Get_PrevStateTag());
}



CPlayerState_DashLeft* CPlayerState_DashLeft::Create(CPlayer* pPlayer)
{
    CPlayerState_DashLeft* pInstance = new CPlayerState_DashLeft();

    /* 원형객체를 초기화한다.  */
    if (FAILED(pInstance->Initialize(pPlayer)))
    {
        MSG_BOX("Failed to Created : CPlayerState_DashLeft");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPlayerState_DashLeft::Free()
{
    __super::Free();
}
