#pragma once
#include "AITask_Base.h"

BEGIN(Client)

class CEN00_Task_KeepEye final : public CAITask_Base
{

public:
	virtual BrainTree::Node::Status		update() override;

private:
	virtual BrainTree::Node::Status		Task() override;

private:
	_bool	m_bMoving = false;	
	_uint	m_iRandomAnimIndex = -1;
};

END


