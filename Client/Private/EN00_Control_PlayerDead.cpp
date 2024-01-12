#include "stdafx.h"
#include "EN00_Control_PlayerDead.h"
#include "GameObject.h"

BrainTree::Node::Status CEN00_Control_PlayerDead::update()
{
	if(true == Control_Function())
		return BrainTree::Node::Status::Success;

	else
		return BrainTree::Node::Status::Failure;
	
}

_bool CEN00_Control_PlayerDead::Control_Function()
{
	if(false == blackboard->GetTarget()->Is_Dead())
		m_bCheck = true;
	else
		m_bCheck = false;


	return m_bCheck;
}
