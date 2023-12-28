#include "..\Public\EN00_Control_Hit.h"
#include "GameObject.h"

BrainTree::Node::Status CEN00_Control_Hit::update()
{
	if(true == Control_Function())
		return BrainTree::Node::Status::Success;

	else
		return BrainTree::Node::Status::Failure;
	
}

_bool CEN00_Control_Hit::Control_Function()
{
	if(true == blackboard->getBool("Is_Hit"))
		m_bCheck = true;

	else
	{
		m_bCheck = false;
	}



	return m_bCheck;
}
