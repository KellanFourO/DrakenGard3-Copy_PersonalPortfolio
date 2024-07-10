#include "stdafx.h"
#include "AITask_Base.h"
#include "Model.h"
#include "Animation.h"
#include "GameObject.h"
#include "Transform.h"
#include "Navigation.h"

void CAITask_Base::SetAnimation(_int iAnimIndex)
{
	if (iAnimIndex != blackboard->GetModel()->Get_CurrentAnimationIndex())
		blackboard->GetModel()->Set_Animation(iAnimIndex);
}

void CAITask_Base::NotLoop()
{
	blackboard->GetModel()->Set_Loop(false);
}

void CAITask_Base::StartLoop()
{
	blackboard->GetModel()->Set_Loop(true);
}

_bool CAITask_Base::EndAnim()
{
	return blackboard->GetModel()->Get_CurrentAnimation()->Get_Finished();
}

void CAITask_Base::OneLook()
{
	if (true == m_bOneTick)
	{
		CTransform* pTransform = blackboard->GetTransform();
		_vector vTargetPos = blackboard->GetTarget()->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		pTransform->Look_At(vTargetPos);

		m_bOneTick = false;
	}

}

void CAITask_Base::KeepEye(CGameObject* pTarget)
{
	_vector vTargetPos = pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	CTransform* pTransform = blackboard->GetTransform();



	_vector vPosition = pTransform->Get_State(CTransform::STATE_POSITION);
	_vector vRight = pTransform->Get_State(CTransform::STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * blackboard->getFloat("Speed") * blackboard->GetTimeDelta();

	
	pTransform->Look_At(vTargetPos);
	pTransform->Set_State(CTransform::STATE_POSITION, vPosition);
}

_int CAITask_Base::Get_RandomNumber(_int MaxCount)
{

	random_device rd;
	mt19937 gen(rd());

	uniform_int_distribution<int> distributionID(0, MaxCount);

	return distributionID(gen);
}
