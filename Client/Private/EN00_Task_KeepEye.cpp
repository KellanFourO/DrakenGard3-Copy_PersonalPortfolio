#include "stdafx.h"
#include "EN00_Task_KeepEye.h"
#include "GameObject.h"
#include "Transform.h"

BrainTree::Node::Status CEN00_Task_KeepEye::update()
{
    if (m_bOneTick)
    {
        _int iRandomNumber = Get_RandomNumber(2);

        if (iRandomNumber == 0)
        {
            SetAnimation(4);
            m_bMoving = true;
        }

        else if (iRandomNumber == 1)
        {
            SetAnimation(5);
            m_bMoving = true;
        }

        else
        {
            SetAnimation(6);
            m_bMoving = false;
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
        pOwnerTransform->Go_Right(blackboard->GetTimeDelta(), blackboard->GetNavigation());
    }


    if (true == EndAnim())
    {
        m_bOneTick = true;
        return BrainTree::Node::Status::Success;
    }
    else
        return BrainTree::Node::Status::Running;
    

}
