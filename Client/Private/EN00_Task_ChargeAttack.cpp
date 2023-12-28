#include "stdafx.h"
#include "EN00_Task_ChargeAttack.h"
#include "GameObject.h"

BrainTree::Node::Status CEN00_Task_ChargeAttack::update()
{
    return Task();
}

BrainTree::Node::Status CEN00_Task_ChargeAttack::Task()
{
    SetAnimation(16);

    if(true == EndAnim())
        return BrainTree::Node::Status::Success;
    else
        return BrainTree::Node::Status::Success;
    
}
