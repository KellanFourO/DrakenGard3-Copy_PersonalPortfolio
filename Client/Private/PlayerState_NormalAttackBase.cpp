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
    ////! �޺������� ������ �޺������� ������ ������ ���ϴ� �ִϸ��̼��� ������ ���� �޺����ݿ� �ش��ϴ� Ű�Է����־��ٸ�.
    //if (true == m_isEnd && true == m_bInput && true == pOwnerModel->Get_CurrentAnimation()->Get_Finished())
    //{
    //    m_pOwnerStateCom->Transition(CStateMachine::STATE_GROUND, strNextComboStateTag);
    //}
    //else if (true == m_isEnd && false == m_bInput && true == pOwnerModel->Get_CurrentAnimation()->Get_Finished())
    //{
    //    m_pOwnerStateCom->Transition(CStateMachine::STATE_GROUND, TEXT("PlayerState_Idle"));
    //}


    ////! ���� �޺����� �ִϸ��̼��� �����ٸ�.
    //if (false == m_isEnd && true == pOwnerModel->Get_CurrentAnimation()->Get_Finished())
    //{
    //    m_isEnd = true; //! m_isEnd�� True�� �Ǽ� ���̻� ���⸦ Ÿ������ ��.
    //    m_pOwnerModelCom->Set_Animation(iEndAnimIndex); //! �޺������� �����ϴ� �ִϸ��̼����� ����.
    //    m_pOwnerModelCom->Set_Loop(false);
    //}

    ////! ���� �޺����� �ִϸ��̼��� ������ ���� �޺����ݿ� �ش��ϴ� Ű�Է��� �־��ٸ�.
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
