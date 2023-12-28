#include "stdafx.h"
#include "EN00_Task_MoveToTarget.h"
#include "GameObject.h"
#include "Transform.h"

BrainTree::Node::Status CEN00_Task_MoveToTarget::update()
{
    return Task();
}

BrainTree::Node::Status CEN00_Task_MoveToTarget::Task()
{
    NotLoop();
    SetAnimation(3);
    
    blackboard->GetTransform()->Set_SpeedPerSec(2.f);
    blackboard->GetTransform()->Set_RotationPerSec(XMConvertToRadians(55.f));
    


    if(true == EndAnim())
        return BrainTree::Node::Status::Success;
    else
        return BrainTree::Node::Status::Running;

    
}
