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
#include "Effect_DashRing.h"
#include "Effect_Hanabira.h"


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

    Create_DashRing();

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
    Create_Hanabira(fTimeDelta);
}
    

void CPlayerState_DashLeft::Late_Tick(const _float& fTimeDelta)
{
    if (true == m_pOwnerModelCom->Get_CurrentAnimation()->Get_Finished())
    {
        wstring strPrevTag = m_pOwnerStateCom->Get_PrevStateTag();

        if (strPrevTag == TEXT("PlayerState_Walk") || strPrevTag == TEXT("PlayerState_Run"))
            m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, strPrevTag);
        else
            m_pOwnerStateCom->Transition(CStateMachine::STATETYPE::STATE_GROUND, TEXT("PlayerState_Idle"));
    }
}

void CPlayerState_DashLeft::Create_DashRing()
{
    CEffect_DashRing::DASHRING_DESC Desc;
    Desc.fLifeTime = 0.3f;
    CGameObject* pPlayer = m_pGameInstance->Get_Player(LEVEL_GAMEPLAY);
    XMStoreFloat4(&Desc.vCreatePos, pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION));

    _vector vPlayerLook = pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK);
    Desc.vCreateLook = pPlayer->Get_Transform()->Get_RotateDir(vPlayerLook, -45.f);
    /*XMStoreFloat4(&Desc.vCreateLook, pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK));*/

    m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_DashRing"), &Desc);
}

void CPlayerState_DashLeft::Create_Hanabira(_float fTimeDelta)
{
    m_fHanabiraAcc += fTimeDelta;

    if (m_fHanabiraAcc >= m_fCreateHabiraTime)
    {




        for (_int i = 0; i < 6; ++i)
        {
            _vector vMyPos = m_pOwnerTransform->Get_State(CTransform::STATE_POSITION);

            _float4 vRandomPos = m_pOwnerTransform->Get_RandomPositionAroundCenter(vMyPos, XMConvertToRadians(-180.f));

            CEffect_Hanabira::HANABIRA_DESC HanabiraDesc;

            HanabiraDesc.eType = CEffect_Hanabira::HANABIRA_MAPLE;
            HanabiraDesc.fLifeTime = 2.f;
            HanabiraDesc.fSpeedPerSec = 5.f;
            HanabiraDesc.vPos = vRandomPos;
            //XMStoreFloat4(&HanabiraDesc.vPos, m_pOwnerTransform->Get_State(CTransform::STATE_POSITION));
            XMStoreFloat4(&HanabiraDesc.vLook, m_pOwnerTransform->Get_State(CTransform::STATE_LOOK));

            m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Hanabira"), &HanabiraDesc);
        }




        m_fHanabiraAcc = 0.f;
    }
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
