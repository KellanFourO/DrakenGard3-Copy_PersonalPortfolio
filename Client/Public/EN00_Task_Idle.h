#pragma once
#include "AITask_Base.h"

BEGIN(Client)

class CEN00_Task_Idle final : public CAITask_Base
{

public:
	virtual BrainTree::Node::Status		update() override;

private:
	virtual BrainTree::Node::Status		Task() override;

};

END


