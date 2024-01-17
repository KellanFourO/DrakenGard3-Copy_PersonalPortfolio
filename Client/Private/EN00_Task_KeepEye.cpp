#include "stdafx.h"
#include "EN00_Task_KeepEye.h"
#include "GameObject.h"
#include "Transform.h"
#include "Navigation.h"

BrainTree::Node::Status CEN00_Task_KeepEye::update()
{
    if (m_bOneTick)
    {
        _int iRandomNumber = Random({0,1,0,1,0,1,0,1});

        if (iRandomNumber == 0)
        {
            m_iRandomAnimIndex = 4;
            SetAnimation(4);
            m_bMoving = true;
        }

        else if (iRandomNumber == 1)
        {
            m_iRandomAnimIndex = 5;
            SetAnimation(5);
            m_bMoving = true;
        }
          
       m_bOneTick = false;
    }

    return Task();
}

BrainTree::Node::Status CEN00_Task_KeepEye::Task()
{
    CTransform* pOwnerTransform = blackboard->GetTransform();
    
    pOwnerTransform->Set_SpeedPerSec(1.5f);
    pOwnerTransform->Set_RotationPerSec(XMConvertToRadians(55.f));

    if (true == m_bMoving)
    {
        pOwnerTransform->Look_At(blackboard->GetTarget()->Get_Transform()->Get_State(CTransform::STATE_POSITION));

        if(4 == m_iRandomAnimIndex)
            pOwnerTransform->KeepEye(blackboard->GetTimeDelta(), false, blackboard->GetNavigation());
        else if(5 == m_iRandomAnimIndex)
            pOwnerTransform->KeepEye(blackboard->GetTimeDelta(), blackboard->GetNavigation());
    }


    if (true == EndAnim())
    {
        m_bOneTick = true;
        return BrainTree::Node::Status::Success;
    }
    else
        return BrainTree::Node::Status::Running;
    

}
