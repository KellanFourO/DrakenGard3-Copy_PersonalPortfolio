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

    if (m_pGameInstance->Key_Pressing(DIK_W) && m_pGameInstance->Key_Down(DIK_SPACE))
    {
        m_ePrevDir = m_eCurrentDir;
        m_eCurrentDir = CPlayerState_Base::FRONT;
        if (m_ePrevDir != m_eCurrentDir)
        {
            Vertical_Camera_Rotate();
        }

        m_pOwnerStateCom->Transition(CStateMachine::STATE_GROUND, TEXT("PlayerState_DashFront"));
    }
    else if (m_pGameInstance->Key_Pressing(DIK_S) && m_pGameInstance->Key_Down(DIK_SPACE))
    {
        m_ePrevDir = m_eCurrentDir;
        m_eCurrentDir = CPlayerState_Base::BACK;
        if (m_ePrevDir != m_eCurrentDir)
        {
            Vertical_Camera_Rotate();
        }

        m_pOwnerStateCom->Transition(CStateMachine::STATE_GROUND, TEXT("PlayerState_DashBack"));
    }

    else if (m_pGameInstance->Key_Pressing(DIK_A) && m_pGameInstance->Key_Down(DIK_SPACE))
    {
        m_ePrevDir = m_eCurrentDir;
        m_eCurrentDir = CPlayerState_Base::LEFT;
        if (m_ePrevDir != m_eCurrentDir)
        {
            Horizon_Camera_Rotate();
        }

        m_pOwnerStateCom->Transition(CStateMachine::STATE_GROUND, TEXT("PlayerState_DashLeft"));
    }

    else if (m_pGameInstance->Key_Pressing(DIK_D) && m_pGameInstance->Key_Down(DIK_SPACE))
    {
        m_ePrevDir = m_eCurrentDir;
        m_eCurrentDir = CPlayerState_Base::RIGHT;
        if (m_ePrevDir != m_eCurrentDir)
        {
            Horizon_Camera_Rotate();
        }

        m_pOwnerStateCom->Transition(CStateMachine::STATE_GROUND, TEXT("PlayerState_DashRight"));
    }

    _float3 vCamLook;
    XMStoreFloat3(&vCamLook, m_pOwnerCam->Get_Transform()->Get_State(CTransform::STATE_LOOK));
       
    m_pOwnerTransform->Look_At_CamLook(vCamLook);

    //! �޺����� �� ���Է��� ����
    if (m_pGameInstance->Mouse_Down(DIM_LB))
    {
        m_bInput = true;
    }

    //! ���� �޺����� �ִϸ��̼��� ����ƴٸ�
    if (false == m_isEnd && true == m_pOwnerModelCom->Get_CurrentAnimation()->Get_Finished())
    {
       //! ���Է��� �־��ٸ�
        if (true == m_bInput)
        {
            pOwnerStateMachine->Transition(CStateMachine::STATE_GROUND, strNextComboStateTag);
        }
        ////! ���Է��� �����ٸ�
        else if(false == m_bInput)
        {
            m_pOwnerColliderCom->OffCollider();
            m_pOwnerModelCom->Root_MotionEnd();
            pOwnerModel->Set_Animation(iEndAnimIndex);
            m_isEnd = true;
        }
            
    }

    //! ���� ������� �ִϸ��̼��� �ٲ���´�. �ٲ� �ִϸ��̼��� �����ٸ� Idel���·� ���ư���
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
