#include "stdafx.h"
#include "EN00_Task_UpperAttack.h"
#include "GameObject.h"

BrainTree::Node::Status CEN00_Task_UpperAttack::update()
{
    return Task();
}

BrainTree::Node::Status CEN00_Task_UpperAttack::Task()
{
    NotLoop();
    SetAnimation(15);
    OneLook();
    if (true == EndAnim())
    {
        m_bOneTick = true;
        return BrainTree::Node::Status::Success;
    }
    else
        return BrainTree::Node::Status::Running;
    
}
