#include "stdafx.h"
#include "EN00_Task_UpperAttack.h"
#include "GameObject.h"

BrainTree::Node::Status CEN00_Task_UpperAttack::update()
{
    return Task();
}

BrainTree::Node::Status CEN00_Task_UpperAttack::Task()
{
    SetAnimation(15);

    if(true == EndAnim())
        return BrainTree::Node::Status::Success;
    else
        return BrainTree::Node::Status::Running;
    
}
