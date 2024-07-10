#pragma once
#include "Client_Defines.h"
#include "BrainTree/BrainTree.h"
#include <iostream>
#include <random>


BEGIN(Client)

class CAIControl_Base abstract : public BrainTree::Node
{

public:
	virtual BrainTree::Node::Status update() = 0;


protected:
	virtual _bool	Control_Function() = 0;
	_int			Get_RandomNumber(_int MaxCount);

protected:
	_bool	m_bCheck = false;
	


};

END

