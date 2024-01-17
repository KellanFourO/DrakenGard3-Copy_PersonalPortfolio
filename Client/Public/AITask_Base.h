#pragma once
#include "Client_Defines.h"
#include "BrainTree/BrainTree.h"
#include <iostream>

BEGIN(Client)

class CAITask_Base abstract : public BrainTree::Node
{

public:
	virtual BrainTree::Node::Status update() = 0;

protected:
	virtual BrainTree::Node::Status Task() = 0;

protected:
	void	SetAnimation(_int iAnimIndex);
	void	NotLoop();
	void	StartLoop();
	_bool	EndAnim();
	void	OneLook();
	void	KeepEye(CGameObject* pTarget);
	_int	Get_RandomNumber(_int MaxCount);
	
protected:
	_bool	m_bOneTick = true;

};

END



