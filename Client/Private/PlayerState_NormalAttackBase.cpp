#include "stdafx.h"
#include "PlayerState_NormalAttackBase.h"
#include "Model.h"
#include "Animation.h"
#include "StateMachine.h"
#include "GameInstance.h"

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
    ////! 콤보공격이 끝났고 콤보공격이 끝나는 동작을 취하는 애니메이션이 끝났고 다음 콤보공격에 해당하는 키입력이있었다면.
    //if (true == m_isEnd && true == m_bInput && true == pOwnerModel->Get_CurrentAnimation()->Get_Finished())
    //{
    //    m_pOwnerStateCom->Transition(CStateMachine::STATE_GROUND, strNextComboStateTag);
    //}
    //else if (true == m_isEnd && false == m_bInput && true == pOwnerModel->Get_CurrentAnimation()->Get_Finished())
    //{
    //    m_pOwnerStateCom->Transition(CStateMachine::STATE_GROUND, TEXT("PlayerState_Idle"));
    //}


    ////! 현재 콤보공격 애니메이션이 끝났다면.
    //if (false == m_isEnd && true == pOwnerModel->Get_CurrentAnimation()->Get_Finished())
    //{
    //    m_isEnd = true; //! m_isEnd가 True가 되서 더이상 여기를 타지않을 것.
    //    m_pOwnerModelCom->Set_Animation(iEndAnimIndex); //! 콤보공격을 종료하는 애니메이션으로 변경.
    //    m_pOwnerModelCom->Set_Loop(false);
    //}

    ////! 현재 콤보공격 애니메이션이 끝났고 다음 콤보공격에 해당하는 키입력이 있었다면.
    //if (true == m_isEnd && false == m_bInput && m_pGameInstance->Mouse_Down(DIM_LB))
    //    m_bInput = true;
    
    return S_OK;
}

HRESULT CPlayerState_NormalAttackBase::EndState()
{
    return S_OK;
}

void CPlayerState_NormalAttackBase::Tick(const _float& fTimeDelta)
{

}

void CPlayerState_NormalAttackBase::NextComboOrIdle(CModel* pOwnerModel, class CStateMachine* pOwnerStateMachine, const wstring& strNextComboStateTag, _int iEndAnimIndex)
{
    if (m_pGameInstance->Mouse_Down(DIM_LB))
    {
        m_bInput = true;
    }

    if (false == m_isEnd && true == m_pOwnerModelCom->Get_CurrentAnimation()->Get_Finished())
    {
        if (true == m_bInput)
        {
            pOwnerStateMachine->Transition(CStateMachine::STATE_GROUND, strNextComboStateTag);
        }
        else if(false == m_isEnd)
        {
            pOwnerModel->Set_Animation(iEndAnimIndex);
            m_isEnd = true;
        }
    }

    if (true == m_isEnd && true == m_pOwnerModelCom->Get_CurrentAnimation()->Get_Finished())
    {
        pOwnerStateMachine->Transition(CStateMachine::STATE_GROUND, TEXT("PlayerState_Idle"));
    }

    
}

void CPlayerState_NormalAttackBase::Free()
{
    __super::Free();
}
