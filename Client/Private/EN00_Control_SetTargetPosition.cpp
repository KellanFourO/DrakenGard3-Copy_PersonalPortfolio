#include "EN00_Control_SetTargetPosition.h"
#include "GameObject.h"
#include "Transform.h"

BrainTree::Node::Status CEN00_Control_SetTargetPosition::update()
{
	if(true == Control_Function())
		return BrainTree::Node::Status::Success;

	else
		return BrainTree::Node::Status::Failure;
	
}

_bool CEN00_Control_SetTargetPosition::Control_Function()
{
	
	if(true == blackboard->getBool("Is_TargetPosition"))
		m_bCheck = true;

	else
		m_bCheck = false;

	return m_bCheck;
}
