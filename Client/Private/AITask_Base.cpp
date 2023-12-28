#include "stdafx.h"
#include "AITask_Base.h"
#include "Model.h"
#include "Animation.h"

void CAITask_Base::SetAnimation(_int iAnimIndex)
{
	if (iAnimIndex != blackboard->GetModel()->Get_CurrentAnimationIndex())
		blackboard->GetModel()->Set_Animation(iAnimIndex);
}

void CAITask_Base::NotLoop()
{
	blackboard->GetModel()->Set_Loop(false);
}

_bool CAITask_Base::EndAnim()
{
	return blackboard->GetModel()->Get_CurrentAnimation()->Get_Finished();
}

_int CAITask_Base::Get_RandomNumber(_int MaxCount)
{

	random_device rd;
	mt19937 gen(rd());

	uniform_int_distribution<int> distributionID(0, MaxCount);

	return distributionID(gen);
}
