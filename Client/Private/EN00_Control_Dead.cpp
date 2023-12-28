#include "..\Public\EN00_Control_Dead.h"
#include "GameObject.h"

BrainTree::Node::Status CEN00_Control_Dead::update()
{
	if(true == Control_Function())
		return BrainTree::Node::Status::Success;

	else
		return BrainTree::Node::Status::Failure;
	
}

_bool CEN00_Control_Dead::Control_Function()
{
	if(0 >= blackboard->getFloat("Current_HP"))
		m_bCheck = true;

	else
	{
		blackboard->setBool("Is_Dead", false);
		m_bCheck = false;
	}


	return m_bCheck;
}
