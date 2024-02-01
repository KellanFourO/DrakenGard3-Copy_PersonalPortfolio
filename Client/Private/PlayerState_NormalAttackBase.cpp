#include "stdafx.h"
#include "PlayerState_NormalAttackBase.h"
#include "Model.h"
#include "Animation.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "Camera_Target.h"
#include "Effect_Trail.h"

CPlayerState_NormalAttackBase::CPlayerState_NormalAttackBase()
{
}

HRESULT CPlayerState_NormalAttackBase::Initialize(CPlayer* pPlayer)
{
    if(FAILED(__super::Initialize(pPlayer)))
        return E_FAIL;


    return S_OK;
}

HRESULT CPlayerState_NormalAttackBase::StartState()
{
    m_pOwnerModelCom->Set_AnimationSpeed(2.f);
    m_pOwnerColliderCom->OnCollider();
    m_pOwnerTrail->Reset_Points();
    m_pOwnerTrail->On_Trail();
    return S_OK;
}

HRESULT CPlayerState_NormalAttackBase::EndState()
{
    m_pOwnerModelCom->Reset_AnimationSpeed();
    m_pOwnerColliderCom->OffCollider();

    
    m_pOwnerTrail->Off_Trail();
    return S_OK;
}

void CPlayerState_NormalAttackBase::Tick(const _float& fTimeDelta)
{

}

void CPlayerState_NormalAttackBase::NextComboOrIdle(CModel* pOwnerModel, class CStateMachine* pOwnerStateMachine, const wstring& strNextComboStateTag, _int iEndAnimIndex)
{
    _float3 vCamLook;
    XMStoreFloat3(&vCamLook, m_pOwnerCam->Get_Transform()->Get_State(CTransform::STATE_LOOK));
       
    m_pOwnerTransform->Look_At_CamLook(vCamLook);

    //! 콤보공격 중 선입력을 받자
    if (m_pGameInstance->Mouse_Down(DIM_LB))
    {
        m_bInput = true;
    }

    //! 현재 콤보공격 애니메이션이 종료됐다면
    if (false == m_isEnd && true == m_pOwnerModelCom->Get_CurrentAnimation()->Get_Finished())
    {
       //! 선입력이 있었다면
        if (true == m_bInput)
        {
            pOwnerStateMachine->Transition(CStateMachine::STATE_GROUND, strNextComboStateTag);
        }
        ////! 선입력이 없었다면
        else if(false == m_bInput)
        {
            m_pOwnerColliderCom->OffCollider();
            m_pOwnerModelCom->Root_MotionEnd();
            pOwnerModel->Set_Animation(iEndAnimIndex);
            m_isEnd = true;
        }
            
    }

    //! 현재 재생중인 애니메이션이 바뀐상태다. 바뀐 애니메이션이 끝났다면 Idel상태로 돌아가자
    if (true == m_isEnd && true == m_pOwnerModelCom->Get_CurrentAnimation()->Get_Finished())
    {
        pOwnerStateMachine->Transition(CStateMachine::STATE_GROUND, TEXT("PlayerState_Idle"));
        m_pOwnerModelCom->Root_MotionEnd();
        
    }

    
}

void CPlayerState_NormalAttackBase::Free()
{
    __super::Free();
}
