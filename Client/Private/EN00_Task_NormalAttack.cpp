#include "stdafx.h"
#include "EN00_Task_NormalAttack.h"
#include "GameObject.h"

BrainTree::Node::Status CEN00_Task_NormalAttack::update()
{
    return Task();
}

BrainTree::Node::Status CEN00_Task_NormalAttack::Task()
{
    if (true == m_bOneTick)
    {
        SetAnimation(7);
        m_bOneTick = false;
    }

    if (false == m_bOneTick && true == EndAnim())
    {
        SetAnimation(8);
        
        if (true == EndAnim())
        {
            m_bOneTick = true;
           return BrainTree::Node::Status::Success;
        }
    }
    else
        return BrainTree::Node::Status::Running;
}
