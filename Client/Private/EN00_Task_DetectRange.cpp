#include "stdafx.h"
#include "EN00_Task_DetectRange.h"
#include "GameObject.h"
#include "Model.h"
#include "Transform.h"

BrainTree::Node::Status CEN00_Task_DetectRange::update()
{
    

    return Task();
}

BrainTree::Node::Status CEN00_Task_DetectRange::Task()
{
    
    SetAnimation(3);
    NotLoop();

    CTransform* pTargetTransform = blackboard->GetTarget()->Get_Transform();
    CTransform* pOwnerTransform = blackboard->GetTransform();

    pOwnerTransform->Set_SpeedPerSec(1.8f);
    pOwnerTransform->Set_RotationPerSec(XMConvertToRadians(60.f));

    _vector vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);

    pOwnerTransform->Go_Target(vTargetPos, blackboard->GetTimeDelta());

        if (true == EndAnim())
        {
            return BrainTree::Node::Status::Success;
        }
        else
            return BrainTree::Node::Status::Running;
    
}
