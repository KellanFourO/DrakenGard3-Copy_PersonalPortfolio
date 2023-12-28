#include "stdafx.h"
#include "EN00_Task_Idle.h"
#include "GameObject.h"

BrainTree::Node::Status CEN00_Task_Idle::update()
{
    return Task();
}

BrainTree::Node::Status CEN00_Task_Idle::Task()
{
    SetAnimation(0);
    
    
    if(true == EndAnim())
        return BrainTree::Node::Status::Success;
    else
        return BrainTree::Node::Status::Running;
}
