#include "EN00_Control_KeepEye.h"
#include "GameObject.h"

BrainTree::Node::Status CEN00_Control_KeepEye::update()
{
	if(true == Control_Function())
		return BrainTree::Node::Status::Success;

	else
		return BrainTree::Node::Status::Failure;
	
}

_bool CEN00_Control_KeepEye::Control_Function()
{
	_int iRandomNumber = Get_RandomNumber(1);


	switch (iRandomNumber)
	{
		case 0:
			m_bCheck = true;
			break;

		case 1:
			m_bCheck = false;
			break;
	}

	
	return m_bCheck;
}
