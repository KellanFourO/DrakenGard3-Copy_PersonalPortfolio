#include "stdafx.h"
#include "EN00_Task_Dead.h"
#include "GameObject.h"

BrainTree::Node::Status CEN00_Task_Dead::update()
{
    return Task();
}

BrainTree::Node::Status CEN00_Task_Dead::Task()
{
    SetAnimation(38);
    blackboard->setBool("Is_Dead", true);

    if(true == EndAnim())
        return BrainTree::Node::Status::Success;
    else
        return BrainTree::Node::Status::Running;

}
