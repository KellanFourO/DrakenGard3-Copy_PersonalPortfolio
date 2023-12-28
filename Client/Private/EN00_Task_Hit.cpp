#include "stdafx.h"
#include "EN00_Task_Hit.h"
#include "GameObject.h"

BrainTree::Node::Status CEN00_Task_Hit::update()
{
    return Task();
}

BrainTree::Node::Status CEN00_Task_Hit::Task()
{
    SetAnimation(26);

    NotLoop();
    
    if(true == EndAnim())
        return BrainTree::Node::Status::Success;
    else
        return BrainTree::Node::Status::Running;
}
