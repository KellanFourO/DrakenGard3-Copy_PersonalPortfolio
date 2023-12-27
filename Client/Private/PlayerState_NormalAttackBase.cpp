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
        //! ���Է��� �����ٸ�
        else
        {
            pOwnerModel->Set_Animation(iEndAnimIndex);
            m_isEnd = true;
        }
    }

    //! ���� ������� �ִϸ��̼��� �ٲ���´�. �ٲ� �ִϸ��̼��� �����ٸ� Idel���·� ���ư���
    if (true == m_isEnd && true == m_pOwnerModelCom->Get_CurrentAnimation()->Get_Finished())
    {
        pOwnerStateMachine->Transition(CStateMachine::STATE_GROUND, TEXT("PlayerState_Idle"));
    }

    
}

void CPlayerState_NormalAttackBase::Free()
{
    __super::Free();
}
