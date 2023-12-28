#include "EN00_Control_ChargeAttack.h"
#include "GameObject.h"

BrainTree::Node::Status CEN00_Control_ChargeAttack::update()
{
	if(true == Control_Function())
		return BrainTree::Node::Status::Success;

	else
		return BrainTree::Node::Status::Failure;
	
}

_bool CEN00_Control_ChargeAttack::Control_Function()
{
	m_bCheck = true;

	return m_bCheck;
}
