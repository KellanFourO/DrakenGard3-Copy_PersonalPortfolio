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


    if (true == blackboard->getBool("Is_Patrol"))
    {
        return BrainTree::Node::Status::Success;
    }
    else
        return BrainTree::Node::Status::Failure;
    
    
    
}
