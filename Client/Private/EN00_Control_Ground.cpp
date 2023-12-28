#include "EN00_Control_Ground.h"
#include "GameObject.h"

BrainTree::Node::Status CEN00_Control_Ground::update()
{
	if(true == Control_Function())
		return BrainTree::Node::Status::Success;

	else
		return BrainTree::Node::Status::Failure;
	
}

_bool CEN00_Control_Ground::Control_Function()
{
	if(true == blackboard->getBool("Is_Ground"))
		return true;

	else
		return false;
}
