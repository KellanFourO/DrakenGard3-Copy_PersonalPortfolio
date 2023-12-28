#pragma once
#include "AIControl_Base.h"



BEGIN(Client)

class CEN00_Control_DetectRange final : public CAIControl_Base
{

public:
	BrainTree::Node::Status update() override;
	
private:
	virtual _bool			Control_Function() override;

};

END

